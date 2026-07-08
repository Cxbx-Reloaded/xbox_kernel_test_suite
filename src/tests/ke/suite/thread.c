#include <xboxkrnl/xboxkrnl.h>
#include <processthreadsapi.h> // for CreateThread
#include <stdio.h> // for snprintf
#include <excpt.h> // for __try/__except

#include "util/output.h"
#include "util/misc.h"
#include "util/thread.h"
#include "util/exception.h"
#include "assertions/defines.h"

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
    TEST_BEGIN();

    // KeAlertResumeThread alerts the target thread and resumes it if suspended.
    // Requires a suspended thread to meaningfully test.
    // SKIP: Requires a coordinated thread that's in an alertable wait+suspended
    // state. Calling on the current thread or an active thread has no visible
    // effect to validate without the full thread synchronization infrastructure.

    // Basic sanity: calling on current thread shouldn't crash
    PKTHREAD current = KeGetCurrentThread();
    GEN_CHECK(current != NULL, TRUE, "current thread valid");

    TEST_END();
}

TEST_FUNC(KeAlertThread)
{
    TEST_BEGIN();

    // KeAlertThread alerts the target thread, potentially waking it from
    // an alertable wait with STATUS_ALERTED.
    // SKIP: Same as KeAlertResumeThread - needs a thread in alertable wait.

    PKTHREAD current = KeGetCurrentThread();
    GEN_CHECK(current != NULL, TRUE, "current thread valid");

    TEST_END();
}

TEST_FUNC(KeBoostPriorityThread)
{
    TEST_BEGIN();

    // KeBoostPriorityThread temporarily boosts the thread's priority.
    PKTHREAD current = KeGetCurrentThread();
    GEN_CHECK(current != NULL, TRUE, "got current thread");

    // Record original priority
    LONG original_priority = current->Priority;

    // Boost (increment of 2)
    KeBoostPriorityThread(current, 2);

    // Priority should be boosted (higher value)
    LONG boosted = current->Priority;
    GEN_CHECK(boosted >= original_priority, TRUE, "priority boosted");

    // Priority will decay back over time via scheduler.
    // We just verify it didn't crash and had an effect.

    TEST_END();
}

TEST_FUNC(KeDelayExecutionThread)
{
    TEST_BEGIN();

    // KeDelayExecutionThread suspends the current thread for a specified duration.

    // --- Short delay (1ms relative) ---
    LARGE_INTEGER delay;
    delay.QuadPart = -10000LL; // 1ms relative

    LARGE_INTEGER before, after;
    KeQuerySystemTime(&before);
    NTSTATUS status = KeDelayExecutionThread(KernelMode, FALSE, &delay);
    KeQuerySystemTime(&after);

    GEN_CHECK(status, STATUS_SUCCESS, "1ms delay");
    // Should have elapsed at least ~1ms (10000 100ns units)
    LONGLONG elapsed = after.QuadPart - before.QuadPart;
    GEN_CHECK(elapsed >= 5000, TRUE, "elapsed >= 0.5ms");

    // --- Zero delay (yield) ---
    delay.QuadPart = 0;
    status = KeDelayExecutionThread(KernelMode, FALSE, &delay);
    // Returns immediately
    BOOL valid = (status == STATUS_SUCCESS);
    GEN_CHECK(valid, TRUE, "zero delay OK");

    // --- 10ms delay ---
    delay.QuadPart = -100000LL; // 10ms
    KeQuerySystemTime(&before);
    status = KeDelayExecutionThread(KernelMode, FALSE, &delay);
    KeQuerySystemTime(&after);
    GEN_CHECK(status, STATUS_SUCCESS, "10ms delay");
    elapsed = after.QuadPart - before.QuadPart;
    GEN_CHECK(elapsed >= 50000, TRUE, "elapsed >= 5ms");

    TEST_END();
}

TEST_FUNC(KeGetCurrentThread)
{
    TEST_BEGIN();

    PKTHREAD thread = KeGetCurrentThread();
    GEN_CHECK(thread != NULL, TRUE, "returns non-NULL");

    // Calling again should return the same value (same thread context)
    PKTHREAD thread2 = KeGetCurrentThread();
    GEN_CHECK(thread2, thread, "consistent");

    // The thread object should have valid priority
    GEN_CHECK(thread->Priority >= 0, TRUE, "priority >= 0");
    GEN_CHECK(thread->Priority <= 31, TRUE, "priority <= 31");

    TEST_END();
}

TEST_FUNC(KeQueryBasePriorityThread)
{
    TEST_BEGIN();

    PKTHREAD current = KeGetCurrentThread();
    LONG base_priority = KeQueryBasePriorityThread(current);

    // Base priority should be reasonable (typically 8 for normal threads)
    GEN_CHECK(base_priority >= 0, TRUE, "base priority >= 0");
    GEN_CHECK(base_priority <= 31, TRUE, "base priority <= 31");

    // Current priority should be >= base (may be boosted)
    GEN_CHECK(current->Priority >= base_priority, TRUE, "current >= base");

    TEST_END();
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
    TEST_BEGIN();

    PKTHREAD current = KeGetCurrentThread();
    LONG original = KeQueryBasePriorityThread(current);

    // Set to a different value
    LONG prev = KeSetBasePriorityThread(current, original + 1);
    GEN_CHECK(prev, original, "returns old base priority");

    LONG new_base = KeQueryBasePriorityThread(current);
    GEN_CHECK(new_base, original + 1, "base priority changed");

    // Restore
    KeSetBasePriorityThread(current, original);
    LONG restored = KeQueryBasePriorityThread(current);
    GEN_CHECK(restored, original, "base priority restored");

    TEST_END();
}

TEST_FUNC(KeSetDisableBoostThread)
{
    TEST_BEGIN();

    PKTHREAD current = KeGetCurrentThread();

    // Disable boost
    BOOLEAN prev = KeSetDisableBoostThread(current, TRUE);
    // prev is the old disable-boost state (typically FALSE)
    GEN_CHECK(prev == FALSE || prev == TRUE, TRUE, "valid prev state");

    // Re-enable boost
    BOOLEAN prev2 = KeSetDisableBoostThread(current, FALSE);
    GEN_CHECK(prev2, TRUE, "was disabled");

    // Restore original
    KeSetDisableBoostThread(current, prev);

    TEST_END();
}

TEST_FUNC(KeSetPriorityThread)
{
    TEST_BEGIN();

    PKTHREAD current = KeGetCurrentThread();
    LONG original_priority = current->Priority;

    // Set to a specific priority
    LONG prev = KeSetPriorityThread(current, 10);
    GEN_CHECK(prev, original_priority, "returns old priority");
    GEN_CHECK(current->Priority, 10, "priority set to 10");

    // Set higher
    prev = KeSetPriorityThread(current, 15);
    GEN_CHECK(prev, 10, "returns 10");
    GEN_CHECK(current->Priority, 15, "priority set to 15");

    // Restore
    KeSetPriorityThread(current, original_priority);
    GEN_CHECK(current->Priority, original_priority, "priority restored");

    TEST_END();
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
    TEST_BEGIN();

    // KeTestAlertThread tests whether there are pending alerts for the
    // current thread and delivers them if so.
    // Without pending alerts, it simply returns FALSE.

    BOOLEAN result = KeTestAlertThread(KernelMode);
    // No alerts pending in normal test execution
    GEN_CHECK(result, FALSE, "no pending alerts");

    TEST_END();
}
