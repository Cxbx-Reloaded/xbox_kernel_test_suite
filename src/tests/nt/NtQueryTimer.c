#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtQueryTimer)
{
    TEST_BEGIN();

    HANDLE handle = NULL;
    NTSTATUS status = NtCreateTimer(&handle, NULL, NotificationTimer);
    GEN_CHECK(status, STATUS_SUCCESS, "create timer");

    if (NT_SUCCESS(status)) {
        TIMER_BASIC_INFORMATION info;

        // --- Query unset timer ---
        status = NtQueryTimer(handle, &info);
        GEN_CHECK(status, STATUS_SUCCESS, "query unset");
        GEN_CHECK(info.TimerState, FALSE, "unset: not signaled");

        // --- Set timer and query ---
        LARGE_INTEGER due_time;
        due_time.QuadPart = -10000000LL * 60; // 60 seconds
        NtSetTimerEx(handle, &due_time, NULL, KernelMode, NULL, FALSE, 0, NULL);

        status = NtQueryTimer(handle, &info);
        GEN_CHECK(status, STATUS_SUCCESS, "query pending");
        GEN_CHECK(info.TimerState, FALSE, "pending: not signaled");
        // RemainingTime should be positive (time until expiry)
        GEN_CHECK(info.RemainingTime.QuadPart != 0, TRUE, "remaining time non-zero");

        // --- Set immediate timer, wait, then query ---
        due_time.QuadPart = -1; // immediate
        NtSetTimerEx(handle, &due_time, NULL, KernelMode, NULL, FALSE, 0, NULL);
        LARGE_INTEGER wait_timeout;
        wait_timeout.QuadPart = -10000000LL; // 1 second
        NtWaitForSingleObject(handle, FALSE, &wait_timeout);

        status = NtQueryTimer(handle, &info);
        GEN_CHECK(status, STATUS_SUCCESS, "query fired");
        GEN_CHECK(info.TimerState, TRUE, "fired: signaled");

        NtClose(handle);
    }

    // --- Invalid handle ---
    TIMER_BASIC_INFORMATION info;
    status = NtQueryTimer((HANDLE)0xDEAD, &info);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle");

    TEST_END();
}
