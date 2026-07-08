#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtClearEvent)
{
    TEST_BEGIN();

    // Create a signaled event
    HANDLE handle = NULL;
    NTSTATUS status = NtCreateEvent(&handle, NULL, NotificationEvent, TRUE);
    GEN_CHECK(status, STATUS_SUCCESS, "create signaled event");

    if (NT_SUCCESS(status)) {
        // Verify it's signaled
        EVENT_BASIC_INFORMATION info;
        NtQueryEvent(handle, &info);
        GEN_CHECK(info.EventState, 1, "initially signaled");

        // Clear it
        status = NtClearEvent(handle);
        GEN_CHECK(status, STATUS_SUCCESS, "NtClearEvent succeeds");

        // Verify it's now non-signaled
        NtQueryEvent(handle, &info);
        GEN_CHECK(info.EventState, 0, "now non-signaled");

        // Clear again (already non-signaled) - should still succeed
        status = NtClearEvent(handle);
        GEN_CHECK(status, STATUS_SUCCESS, "clear again OK");

        NtQueryEvent(handle, &info);
        GEN_CHECK(info.EventState, 0, "still non-signaled");

        NtClose(handle);
    }

    // --- Invalid handle ---
    status = NtClearEvent((HANDLE)0xDEAD);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle fails");

    TEST_END();
}
