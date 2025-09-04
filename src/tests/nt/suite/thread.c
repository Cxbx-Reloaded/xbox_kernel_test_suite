#include <xboxkrnl/xboxkrnl.h>
#include <processthreadsapi.h>
#include <stdio.h>

#include "util/output.h"
#include "util/misc.h"
#include "util/thread.h"
#include "assertions/defines.h"


TEST_FUNC(NtQueueApcThread)
{
    /* FIXME: This is a stub! implement this function! */
}

typedef struct {
    BOOL run;
    ULONG counter;
    HANDLE hEventSecondary;
    HANDLE hEventPrimary;
} thread_counter_s;

typedef NTSTATUS NTAPI (*threadFuncAPI)(HANDLE, PULONG);

typedef struct _thread_test {
    // Previous result check
    BOOL addThreadCounter;        // 0 or 1
    DWORD expected_resultWait;    // WAIT_TIMEOUT or WAIT_OBJECT_0
    // Next result task
    threadFuncAPI nextFuncAPI;    // NtResumeThread or NtSuspendThread
    // Don't need individual check result value, it is always STATUS_SUCCESS or
    // STATUS_INVALID_HANDLE for wrong handle input
    ULONG expected_suspendCount;  // Return suspend count after call
    ULONG expected_threadCounter; // counter + addThreadCounter dynamic
    ULONG return_threadCounter;
    DWORD return_resultWait;
    ULONG return_suspendCount;
} thread_test;

static DWORD NTAPI NtResumeSuspendThread_secondary(void* arg)
{
    thread_counter_s* thread_data = (thread_counter_s*)arg;
    do {
        THREAD_DUO_EVENTS_WAIT_SECONDARY(thread_data->hEventSecondary);
        thread_data->counter++;

        // Let other thread to run
        THREAD_DUO_EVENTS_NOTIFY(thread_data->hEventSecondary, thread_data->hEventPrimary);
    } while(thread_data->run);
    return 0;
}

static BOOL NtResumeSuspendThreadInline(const char* test_name, BOOL suspend, thread_test* thread_tests, unsigned total)
{
    ASSERT_HEADER;

    THREAD_DUO_EVENTS_CREATE(hEventMain, hEventThread);

    thread_counter_s thread_data = {
        .run = TRUE,
        .counter = 0,
        .hEventSecondary = hEventThread,
        .hEventPrimary = hEventMain
    };

    ULONG old_suspend_count, counter = 0;
    NTSTATUS result;
    DWORD result_wait;

    if (suspend) {
        result = NtSuspendThread(NULL, NULL);
        GEN_CHECK(result, STATUS_INVALID_HANDLE, "result");
        result = NtSuspendThread((HANDLE)0xDEADBEEF, NULL);
        GEN_CHECK(result, STATUS_INVALID_HANDLE, "result");
    }
    else {
        result = NtResumeThread(NULL, NULL);
        GEN_CHECK(result, STATUS_INVALID_HANDLE, "result");
        result = NtResumeThread((HANDLE)0xDEADBEEF, NULL);
        GEN_CHECK(result, STATUS_INVALID_HANDLE, "result");
    }

    HANDLE hThread = CreateThread(NULL, 0, NtResumeSuspendThread_secondary, (void*)&thread_data, (suspend ? CREATE_SUSPENDED : 0), NULL);
    GEN_CHECK(hThread != NULL, TRUE, "valid handle");
    if(!hThread) {
        print("  ERROR: Did not create thread");
        TEST_FAILED();
        THREAD_DUO_EVENTS_DESTROY(hEventMain, hEventThread);
        ASSERT_FOOTER(test_name);
    }

    // Run list of tests
    for (unsigned i =  0; i < total; i++) {
        thread_test* test = &thread_tests[i];
        THREAD_DUO_EVENTS_NOTIFY(hEventMain, hEventThread);

        // Can only check previous state begin
        result_wait = THREAD_DUO_EVENTS_WAIT_PRIMARY(hEventMain);
        test->return_resultWait = result_wait;

        test->expected_threadCounter = counter += test->addThreadCounter;
        test->return_threadCounter = thread_data.counter;
        // Can only check previous state end

        if (test->nextFuncAPI) {
            result = test->nextFuncAPI(hThread, &old_suspend_count);
            test->return_suspendCount = old_suspend_count;
            GEN_CHECK(result, STATUS_SUCCESS, "result");
        }
    }

    GEN_CHECK_ARRAY_MEMBER(thread_tests, return_threadCounter, expected_threadCounter, total, "thread_tests");
    GEN_CHECK_ARRAY_MEMBER(thread_tests, expected_resultWait, return_resultWait, total, "thread_tests");
    GEN_CHECK_ARRAY_MEMBER(thread_tests, expected_suspendCount, return_suspendCount, total, "thread_tests");

    // End of test, tell the other thread to terminate
    thread_data.run = FALSE;
    THREAD_DUO_EVENTS_NOTIFY(hEventMain, hEventThread);

    // Perform the clean up process requirement
    (void)CloseHandle(hThread);
    THREAD_DUO_EVENTS_DESTROY(hEventMain, hEventThread);
    ASSERT_FOOTER(test_name);
}

TEST_FUNC(NtResumeThread)
{
    TEST_BEGIN();

    thread_test thread_tests[] = {
        // Verify if the thread is running then suspend the thread
        { .addThreadCounter = 1, .expected_resultWait = WAIT_OBJECT_0, .nextFuncAPI = NtSuspendThread, .expected_suspendCount = 0},
        // Verify if the thread is suspended then suspend the thread again
        { .addThreadCounter = 0, .expected_resultWait = WAIT_TIMEOUT, .nextFuncAPI = NtSuspendThread, .expected_suspendCount = 1},
        // Verify if the thread is suspended then try resume the thread
        { .addThreadCounter = 0, .expected_resultWait = WAIT_TIMEOUT, .nextFuncAPI = NtResumeThread, .expected_suspendCount = 2},
        // Verify if the thread is suspended then resume the thread
        { .addThreadCounter = 0, .expected_resultWait = WAIT_TIMEOUT, .nextFuncAPI = NtResumeThread, .expected_suspendCount = 1},
        // Verify if the thread is resumed then try resume the thread again
        { .addThreadCounter = 1, .expected_resultWait = WAIT_OBJECT_0, .nextFuncAPI = NtResumeThread, .expected_suspendCount = 0},
    };

    char api_name_i[0x20];
    for (unsigned i = 0; i< 10; i++) {
        snprintf(api_name_i, ARRAY_SIZE(api_name_i), "%s[%d]", TEST_GET_API_NAME, i);
        TEST_GET_VAR &= NtResumeSuspendThreadInline(api_name_i, FALSE, thread_tests, ARRAY_SIZE(thread_tests));
    }

    TEST_END();
}

TEST_FUNC(NtSuspendThread)
{
    TEST_BEGIN();

    thread_test thread_tests[] = {
        // Verify if the thread is suspended then resume the thread
        { .addThreadCounter = 0, .expected_resultWait = WAIT_TIMEOUT, .nextFuncAPI = NtResumeThread, .expected_suspendCount = 1},
        // Verify if the thread is running then try resume the thread
        { .addThreadCounter = 1, .expected_resultWait = WAIT_OBJECT_0, .nextFuncAPI = NtResumeThread, .expected_suspendCount = 0},
        // Verify if the thread is running then try resume the thread again
        { .addThreadCounter = 1, .expected_resultWait = WAIT_OBJECT_0, .nextFuncAPI = NtResumeThread, .expected_suspendCount = 0},
        // Verify if the thread is running then suspend the thread
        { .addThreadCounter = 1, .expected_resultWait = WAIT_OBJECT_0, .nextFuncAPI = NtSuspendThread, .expected_suspendCount = 0},
        // Verify if the thread is running then try suspend the thread
        { .addThreadCounter = 0, .expected_resultWait = WAIT_TIMEOUT, .nextFuncAPI = NtSuspendThread, .expected_suspendCount = 1},
    };

    char api_name_i[0x20];
    for (unsigned i = 0; i< 10; i++) {
        snprintf(api_name_i, ARRAY_SIZE(api_name_i), "%s[%d]", TEST_GET_API_NAME, i);
        TEST_GET_VAR &= NtResumeSuspendThreadInline(api_name_i, TRUE, thread_tests, ARRAY_SIZE(thread_tests));
    }

    TEST_END();
}
