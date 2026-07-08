#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtCancelTimer)
{
    TEST_BEGIN();

    HANDLE handle = NULL;
    NTSTATUS status = NtCreateTimer(&handle, NULL, NotificationTimer);
    GEN_CHECK(status, STATUS_SUCCESS, "create timer");

    if (NT_SUCCESS(status)) {
        // Cancel a timer that was never set
        BOOLEAN was_set = TRUE;
        status = NtCancelTimer(handle, &was_set);
        GEN_CHECK(status, STATUS_SUCCESS, "cancel unset timer");
        GEN_CHECK(was_set, FALSE, "was not set");

        // Set the timer to fire far in the future
        LARGE_INTEGER due_time;
        due_time.QuadPart = -10000000LL * 60; // 60 seconds relative
        BOOLEAN prev_state = FALSE;
        status = NtSetTimerEx(handle, &due_time, NULL, KernelMode, NULL, FALSE, 0, &prev_state);
        GEN_CHECK(status, STATUS_SUCCESS, "set timer");
        GEN_CHECK(prev_state, FALSE, "was not signaled before set");

        // Timer should now be set (query)
        TIMER_BASIC_INFORMATION info;
        NtQueryTimer(handle, &info);
        // TimerState=FALSE means not yet signaled (still pending)
        GEN_CHECK(info.TimerState, FALSE, "pending not yet signaled");

        // Cancel it
        was_set = FALSE;
        status = NtCancelTimer(handle, &was_set);
        GEN_CHECK(status, STATUS_SUCCESS, "cancel set timer");
        GEN_CHECK(was_set, TRUE, "was set");

        // Cancel again - should report not set
        was_set = TRUE;
        status = NtCancelTimer(handle, &was_set);
        GEN_CHECK(status, STATUS_SUCCESS, "cancel again");
        GEN_CHECK(was_set, FALSE, "not set anymore");

        // NULL CurrentState is allowed
        status = NtCancelTimer(handle, NULL);
        GEN_CHECK(status, STATUS_SUCCESS, "NULL state OK");

        NtClose(handle);
    }

    // --- Invalid handle ---
    BOOLEAN dummy;
    status = NtCancelTimer((HANDLE)0xDEAD, &dummy);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle");

    TEST_END();
}
