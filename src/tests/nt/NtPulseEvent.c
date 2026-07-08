#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtPulseEvent)
{
    TEST_BEGIN();

    // NtPulseEvent atomically sets the event to signaled, releases waiters,
    // then resets it to non-signaled.

    HANDLE handle = NULL;
    NTSTATUS status = NtCreateEvent(&handle, NULL, NotificationEvent, FALSE);
    GEN_CHECK(status, STATUS_SUCCESS, "create event");

    if (NT_SUCCESS(status)) {
        // Pulse from non-signaled state
        LONG prev_state = -1;
        status = NtPulseEvent(handle, &prev_state);
        GEN_CHECK(status, STATUS_SUCCESS, "pulse succeeds");
        GEN_CHECK(prev_state, 0, "prev was non-signaled");

        // After pulse, event should be non-signaled
        EVENT_BASIC_INFORMATION info;
        NtQueryEvent(handle, &info);
        GEN_CHECK(info.EventState, 0, "non-signaled after pulse");

        // Set the event, then pulse - should return prev=1 and end non-signaled
        NtSetEvent(handle, NULL);
        prev_state = -1;
        status = NtPulseEvent(handle, &prev_state);
        GEN_CHECK(status, STATUS_SUCCESS, "pulse from signaled");
        GEN_CHECK(prev_state, 1, "prev was signaled");

        NtQueryEvent(handle, &info);
        GEN_CHECK(info.EventState, 0, "non-signaled after second pulse");

        // NULL PreviousState
        status = NtPulseEvent(handle, NULL);
        GEN_CHECK(status, STATUS_SUCCESS, "NULL prev OK");

        NtClose(handle);
    }

    // --- Invalid handle ---
    LONG dummy;
    status = NtPulseEvent((HANDLE)0xDEAD, &dummy);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle fails");

    TEST_END();
}
