#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtSetTimerEx)
{
    TEST_BEGIN();

    HANDLE handle = NULL;
    NTSTATUS status = NtCreateTimer(&handle, NULL, NotificationTimer);
    GEN_CHECK(status, STATUS_SUCCESS, "create timer");

    if (NT_SUCCESS(status)) {
        // --- Set with relative due time (far future) ---
        LARGE_INTEGER due_time;
        due_time.QuadPart = -10000000LL * 30; // 30 seconds
        BOOLEAN prev_state = TRUE;
        status = NtSetTimerEx(handle, &due_time, NULL, KernelMode, NULL, FALSE, 0, &prev_state);
        GEN_CHECK(status, STATUS_SUCCESS, "set relative 30s");
        GEN_CHECK(prev_state, FALSE, "prev not signaled");

        // Timer is now pending
        TIMER_BASIC_INFORMATION info;
        NtQueryTimer(handle, &info);
        GEN_CHECK(info.TimerState, FALSE, "pending");

        // --- Re-set with a very short due time (should fire quickly) ---
        due_time.QuadPart = -1; // 100ns relative (essentially immediate)
        prev_state = TRUE;
        status = NtSetTimerEx(handle, &due_time, NULL, KernelMode, NULL, FALSE, 0, &prev_state);
        GEN_CHECK(status, STATUS_SUCCESS, "re-set immediate");
        // prev_state reports the signal state of the timer before re-setting
        // The timer was pending (not signaled) so prev should be FALSE
        GEN_CHECK(prev_state, FALSE, "prev still not signaled");

        // Wait for it to signal
        LARGE_INTEGER wait_timeout;
        wait_timeout.QuadPart = -10000000LL; // 1 second max
        status = NtWaitForSingleObject(handle, FALSE, &wait_timeout);
        GEN_CHECK(status, STATUS_SUCCESS, "wait for signal");

        // Now it should be signaled
        NtQueryTimer(handle, &info);
        GEN_CHECK(info.TimerState, TRUE, "signaled after fire");

        // --- Set periodic timer ---
        due_time.QuadPart = -10000000LL * 60; // 60s (won't fire during test)
        prev_state = FALSE;
        status = NtSetTimerEx(handle, &due_time, NULL, KernelMode, NULL, FALSE, 1000, &prev_state);
        GEN_CHECK(status, STATUS_SUCCESS, "set periodic");
        GEN_CHECK(prev_state, TRUE, "prev was signaled");

        // Cancel to clean up
        NtCancelTimer(handle, NULL);

        // --- NULL PreviousState ---
        due_time.QuadPart = -10000000LL * 60;
        status = NtSetTimerEx(handle, &due_time, NULL, KernelMode, NULL, FALSE, 0, NULL);
        GEN_CHECK(status, STATUS_SUCCESS, "NULL prev OK");
        NtCancelTimer(handle, NULL);

        NtClose(handle);
    }

    // --- Invalid handle ---
    LARGE_INTEGER due;
    due.QuadPart = -10000000LL;
    status = NtSetTimerEx((HANDLE)0xDEAD, &due, NULL, KernelMode, NULL, FALSE, 0, NULL);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle");

    TEST_END();
}
