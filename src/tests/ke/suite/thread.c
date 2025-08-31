#include <xboxkrnl/xboxkrnl.h>
#include <processthreadsapi.h> // for CreateThread
#include <stdio.h> // for snprintf
#include <excpt.h> // for __try/__except

#include "util/output.h"
#include "util/misc.h"
#include "util/thread.h"
#include "util/exception.h"
#include "assertions/defines.h"

// TODO: Add below into nxdk's xboxkrnl/ntstatus.h file
#define STATUS_SUSPEND_COUNT_EXCEEDED 0xC000004A

typedef struct {
    BOOL terminate;
    ULONG counter;
    HANDLE hEventThread;
    HANDLE hEventMain;
} thread_sync_counter_s;

typedef ULONG (NTAPI *threadFuncAPI) (PKTHREAD);

typedef struct _thread_test {
    // Previous status check
    BOOL addThreadCounter;        // 0 or 1
    NTSTATUS expected_statusWait; // STATUS_TIMEOUT or STATUS_WAIT_0
    // Next status task
    threadFuncAPI nextFuncAPI;    // KeResumeThread or KeSuspendThread
    // Don't need individual check status value, it is always STATUS_SUCCESS or
    // STATUS_INVALID_HANDLE for wrong handle input
    ULONG expected_suspendCount;  // Return suspend count after call
    ULONG expected_threadCounter; // counter + addThreadCounter dynamic
    ULONG return_threadCounter;
    NTSTATUS return_statusWait;
    ULONG return_suspendCount;
} thread_test;

static DWORD NTAPI KeResumeSuspendThread_sync(void* arg)
{
    thread_sync_counter_s* thread_data = (thread_sync_counter_s*)arg;
    NTSTATUS status;
    while (1) {
        THREAD_DUO_EVENTS_WAIT(thread_data, thread_data->hEventThread, status);

        thread_data->counter++;

        // Let other thread to run
        THREAD_DUO_EVENTS_SET(thread_data->hEventMain);
    }
    return 0;
}

static BOOL KeResumeSuspendThreadInline(const char* test_name, BOOL suspend, thread_test* thread_tests, unsigned total)
{
    ASSERT_HEADER;

    HANDLE hEventMain, hEventThread;
    THREAD_DUO_EVENTS_CREATE(hEventMain, hEventThread, FALSE);

    thread_sync_counter_s thread_data = {
        .terminate = FALSE,
        .counter = 0,
        .hEventThread = hEventThread,
        .hEventMain = hEventMain
    };

    ULONG old_suspend_count, counter = 0;
    NTSTATUS status;


#if 0 // TODO: See ticket https://github.com/Cxbx-Reloaded/xbox_kernel_test_suite/issues/111 for what could be done to able test invalid arguments
    // NOTE: __try/__except cannot be used here due to the kernel function call to KeRaiseIrqlToDpcLevel, which is not exception safe.
    if (suspend) {
        BOOL catch = -1;
        __try {
            (void)KeSuspendThread(NULL);
            catch = 0;
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            catch = 1;
        }
        GEN_CHECK(catch, 1, "catch");
        catch = -1;
        __try {
            (void)KeSuspendThread((PKTHREAD)0xBEEF);
            catch = 0;
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            catch = 1;
        }
        GEN_CHECK(catch, 1, "catch");
    }
    else {
        BOOL catch = -1;
        __try {
            (void)KeResumeThread(NULL);
            catch = 0;
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            catch = 1;
        }
        GEN_CHECK(catch, 1, "catch");
        catch = -1;
        __try {
            (void)KeResumeThread((PKTHREAD)0xBEEF);
            catch = 0;
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            catch = 1;
        }
        GEN_CHECK(catch, 1, "catch");
    }
#endif

    HANDLE hThread = CreateThread(NULL, 0, KeResumeSuspendThread_sync, (void*)&thread_data, (suspend ? CREATE_SUSPENDED : 0), NULL);
    GEN_CHECK(hThread != NULL, TRUE, "valid handle");
    if(!hThread) {
        print("  ERROR: Did not create thread");
        TEST_FAILED();
        THREAD_DUO_EVENTS_DESTROY(hEventMain, hEventThread);
        ASSERT_FOOTER(test_name);
    }
    // Since we are checking Ke functions, we need the actual thread object.
    PETHREAD Thread;
    status = ObReferenceObjectByHandle(hThread, &PsThreadObjectType, (PVOID*)&Thread);
    GEN_CHECK(NT_SUCCESS(status), TRUE, "status");

    // Run list of tests
    for (unsigned i =  0; i < total; i++) {
        thread_test* test = &thread_tests[i];
        THREAD_DUO_EVENTS_SET(hEventThread);

        // Can only check previous state begin
        THREAD_DUO_EVENTS_WAIT_GET_STATUS(hEventMain, test->return_statusWait);

        test->expected_threadCounter = counter += test->addThreadCounter;
        test->return_threadCounter = thread_data.counter;
        // Can only check previous state end

        if (test->nextFuncAPI) {
            old_suspend_count = test->nextFuncAPI(&Thread->Tcb);
            test->return_suspendCount = old_suspend_count;
        }
    }

    GEN_CHECK_ARRAY_MEMBER(thread_tests, return_threadCounter, expected_threadCounter, total, "thread_tests");
    GEN_CHECK_ARRAY_MEMBER(thread_tests, expected_statusWait, return_statusWait, total, "thread_tests");
    GEN_CHECK_ARRAY_MEMBER(thread_tests, expected_suspendCount, return_suspendCount, total, "thread_tests");

    if (suspend) {
        // Test for maximum suspended count limit
        ULONG suspend_count = 0, suspend_count_old;
        BOOL is_caught = FALSE;
        __try {
            do {
                suspend_count = KeSuspendThread(&Thread->Tcb);
                suspend_count++; // We want to get up-to-date count instead of previous count from called function
            } while (suspend_count < 0x80);
            is_caught = -1;
        }
        // We should be able to catch exception here since KeSuspendThread does throw an exception
        __except (GetExceptionStatus(&status, EXCEPTION_EXECUTE_HANDLER)) {
            // If an exception is caught, then don't increment the count
            is_caught = TRUE;
        }
        GEN_CHECK(is_caught, TRUE, "is_caught");
        GEN_CHECK(suspend_count, 0x7F, "suspend_count_current");
        GEN_CHECK(status, STATUS_SUSPEND_COUNT_EXCEEDED, "status_exception");

        suspend_count = KeResumeThread(&Thread->Tcb); // Require to get the updated suspend count which will start decrement from here.
        do {
            suspend_count_old = suspend_count;
            suspend_count = KeResumeThread(&Thread->Tcb);
        } while (suspend_count < suspend_count_old);
        GEN_CHECK(suspend_count, 0, "suspend_count");
    }

    // End of test, tell the other thread to terminate
    thread_data.terminate = TRUE;
    THREAD_DUO_EVENTS_SET(hEventThread);

    // Perform the clean up process requirement
    THREAD_DUO_EVENTS_DESTROY_THREAD_WAIT(hThread);
    THREAD_DUO_EVENTS_DESTROY(hEventMain, hEventThread);
    ASSERT_FOOTER(test_name);
}

TEST_FUNC(KeAlertResumeThread)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(KeAlertThread)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(KeBoostPriorityThread)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(KeDelayExecutionThread)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(KeGetCurrentThread)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(KeQueryBasePriorityThread)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(KeResumeThread)
{
    TEST_BEGIN();

    thread_test thread_tests[] = {
        // Verify if the thread is running then suspend the thread
        { .addThreadCounter = 1, .expected_statusWait = STATUS_WAIT_0, .nextFuncAPI = KeSuspendThread, .expected_suspendCount = 0},
        // Verify if the thread is suspended then suspend the thread again
        { .addThreadCounter = 0, .expected_statusWait = STATUS_TIMEOUT, .nextFuncAPI = KeSuspendThread, .expected_suspendCount = 1},
        // Verify if the thread is suspended then try resume the thread
        { .addThreadCounter = 0, .expected_statusWait = STATUS_TIMEOUT, .nextFuncAPI = KeResumeThread, .expected_suspendCount = 2},
        // Verify if the thread is suspended then resume the thread
        { .addThreadCounter = 0, .expected_statusWait = STATUS_TIMEOUT, .nextFuncAPI = KeResumeThread, .expected_suspendCount = 1},
        // Verify if the thread is resumed then try resume the thread again
        { .addThreadCounter = 1, .expected_statusWait = STATUS_WAIT_0, .nextFuncAPI = KeResumeThread, .expected_suspendCount = 0},
    };

    char api_name_i[0x20];
    for (unsigned i = 0; i< 10; i++) {
        snprintf(api_name_i, ARRAY_SIZE(api_name_i), "%s[%d]", TEST_GET_API_NAME, i);
        TEST_GET_VAR &= KeResumeSuspendThreadInline(api_name_i, FALSE, thread_tests, ARRAY_SIZE(thread_tests));
    }

    TEST_END();
}

TEST_FUNC(KeSetBasePriorityThread)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(KeSetDisableBoostThread)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(KeSetPriorityThread)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(KeSuspendThread)
{
    TEST_BEGIN();

    thread_test thread_tests[] = {
        // Verify if the thread is suspended then resume the thread
        { .addThreadCounter = 0, .expected_statusWait = STATUS_TIMEOUT, .nextFuncAPI = KeResumeThread, .expected_suspendCount = 1},
        // Verify if the thread is running then try resume the thread
        { .addThreadCounter = 1, .expected_statusWait = STATUS_WAIT_0, .nextFuncAPI = KeResumeThread, .expected_suspendCount = 0},
        // Verify if the thread is running then try resume the thread again
        { .addThreadCounter = 1, .expected_statusWait = STATUS_WAIT_0, .nextFuncAPI = KeResumeThread, .expected_suspendCount = 0},
        // Verify if the thread is running then suspend the thread
        { .addThreadCounter = 1, .expected_statusWait = STATUS_WAIT_0, .nextFuncAPI = KeSuspendThread, .expected_suspendCount = 0},
        // Verify if the thread is suspended then try suspend the thread
        { .addThreadCounter = 0, .expected_statusWait = STATUS_TIMEOUT, .nextFuncAPI = KeSuspendThread, .expected_suspendCount = 1},
        // Verify if the thread is suspended then try resume the thread
        { .addThreadCounter = 0, .expected_statusWait = STATUS_TIMEOUT, .nextFuncAPI = KeResumeThread, .expected_suspendCount = 2},
        // Verify if the thread is suspended then resume the thread
        { .addThreadCounter = 0, .expected_statusWait = STATUS_TIMEOUT, .nextFuncAPI = KeResumeThread, .expected_suspendCount = 1},
        // Verify if the thread is running then try resume the thread
        { .addThreadCounter = 1, .expected_statusWait = STATUS_WAIT_0, .nextFuncAPI = KeResumeThread, .expected_suspendCount = 0},
    };

    char api_name_i[0x20];
    for (unsigned i = 0; i< 10; i++) {
        snprintf(api_name_i, ARRAY_SIZE(api_name_i), "%s[%d]", TEST_GET_API_NAME, i);
        TEST_GET_VAR &= KeResumeSuspendThreadInline(api_name_i, TRUE, thread_tests, ARRAY_SIZE(thread_tests));
    }

    TEST_END();
}

TEST_FUNC(KeTestAlertThread)
{
    /* FIXME: This is a stub! implement this function! */
}
