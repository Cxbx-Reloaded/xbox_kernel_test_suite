#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtCreateTimer)
{
    TEST_BEGIN();

    // --- Notification timer ---
    HANDLE handle = NULL;
    NTSTATUS status = NtCreateTimer(&handle, NULL, NotificationTimer);
    GEN_CHECK(status, STATUS_SUCCESS, "create notification timer");

    if (NT_SUCCESS(status)) {
        GEN_CHECK(handle != NULL, TRUE, "handle valid");

        // Query should show timer not set (not signaled)
        TIMER_BASIC_INFORMATION info;
        status = NtQueryTimer(handle, &info);
        GEN_CHECK(status, STATUS_SUCCESS, "query notification timer");
        GEN_CHECK(info.TimerState, FALSE, "not signaled initially");

        NtClose(handle);
    }

    // --- Synchronization timer ---
    handle = NULL;
    status = NtCreateTimer(&handle, NULL, SynchronizationTimer);
    GEN_CHECK(status, STATUS_SUCCESS, "create sync timer");

    if (NT_SUCCESS(status)) {
        TIMER_BASIC_INFORMATION info;
        status = NtQueryTimer(handle, &info);
        GEN_CHECK(status, STATUS_SUCCESS, "query sync timer");
        GEN_CHECK(info.TimerState, FALSE, "not signaled");

        NtClose(handle);
    }

    TEST_END();
}
