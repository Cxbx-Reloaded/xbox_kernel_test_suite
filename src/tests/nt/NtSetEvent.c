#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtSetEvent)
{
    TEST_BEGIN();

    // Create event in non-signaled state
    HANDLE handle = NULL;
    NTSTATUS status = NtCreateEvent(&handle, NULL, NotificationEvent, FALSE);
    GEN_CHECK(status, STATUS_SUCCESS, "create event");

    if (NT_SUCCESS(status)) {
        // Set it - should return previous state (0 = not signaled)
        LONG prev_state = -1;
        status = NtSetEvent(handle, &prev_state);
        GEN_CHECK(status, STATUS_SUCCESS, "NtSetEvent succeeds");
        GEN_CHECK(prev_state, 0, "prev state was not-signaled");

        // Set again - now previous state should be 1 (signaled)
        prev_state = -1;
        status = NtSetEvent(handle, &prev_state);
        GEN_CHECK(status, STATUS_SUCCESS, "NtSetEvent again");
        GEN_CHECK(prev_state, 1, "prev state was signaled");

        // Verify event is signaled via query
        EVENT_BASIC_INFORMATION info;
        NtQueryEvent(handle, &info);
        GEN_CHECK(info.EventState, 1, "event is signaled");

        // NULL PreviousState is allowed
        status = NtSetEvent(handle, NULL);
        GEN_CHECK(status, STATUS_SUCCESS, "NULL prev_state OK");

        NtClose(handle);
    }

    // --- Invalid handle ---
    LONG dummy;
    status = NtSetEvent((HANDLE)0xDEAD, &dummy);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle fails");

    TEST_END();
}
