#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtSignalAndWaitForSingleObjectEx)
{
    TEST_BEGIN();

    // NtSignalAndWaitForSingleObjectEx atomically signals one object
    // and waits on another.

    HANDLE signal_event = NULL, wait_event = NULL;
    NTSTATUS status;

    status = NtCreateEvent(&signal_event, NULL, NotificationEvent, FALSE);
    GEN_CHECK(status, STATUS_SUCCESS, "create signal event");
    status = NtCreateEvent(&wait_event, NULL, NotificationEvent, TRUE);
    GEN_CHECK(status, STATUS_SUCCESS, "create wait event (signaled)");

    if (signal_event && wait_event) {
        LARGE_INTEGER timeout;
        timeout.QuadPart = 0;

        // Signal signal_event, wait on wait_event (already signaled)
        status = NtSignalAndWaitForSingleObjectEx(
            signal_event, wait_event, KernelMode, FALSE, &timeout);
        GEN_CHECK(status, STATUS_SUCCESS, "signal+wait succeeds");

        // Verify signal_event was signaled
        EVENT_BASIC_INFORMATION info;
        NtQueryEvent(signal_event, &info);
        GEN_CHECK(info.EventState, 1, "signal_event is signaled");

        // Now clear wait_event and try - should timeout
        NtClearEvent(wait_event);
        status = NtSignalAndWaitForSingleObjectEx(
            signal_event, wait_event, KernelMode, FALSE, &timeout);
        GEN_CHECK(status, STATUS_TIMEOUT, "wait timeout");

        NtClose(signal_event);
        NtClose(wait_event);
    }

    TEST_END();
}
