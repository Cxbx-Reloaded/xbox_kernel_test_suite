#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtWaitForSingleObjectEx)
{
    TEST_BEGIN();

    // NtWaitForSingleObjectEx is the extended version with WaitMode parameter.
    // On Xbox, it behaves the same as NtWaitForSingleObject for kernel mode.

    HANDLE event = NULL;
    NTSTATUS status = NtCreateEvent(&event, NULL, NotificationEvent, TRUE);
    GEN_CHECK(status, STATUS_SUCCESS, "create event");

    if (NT_SUCCESS(status)) {
        LARGE_INTEGER timeout;
        timeout.QuadPart = 0;

        // Wait on signaled event - immediate
        status = NtWaitForSingleObjectEx(event, KernelMode, FALSE, &timeout);
        GEN_CHECK(status, STATUS_SUCCESS, "signaled immediate");

        // Clear and wait - timeout
        NtClearEvent(event);
        status = NtWaitForSingleObjectEx(event, KernelMode, FALSE, &timeout);
        GEN_CHECK(status, STATUS_TIMEOUT, "non-signaled timeout");

        // Alertable wait with no pending APCs should still just timeout
        timeout.QuadPart = -10000LL; // 1ms
        status = NtWaitForSingleObjectEx(event, KernelMode, TRUE, &timeout);
        GEN_CHECK(status, STATUS_TIMEOUT, "alertable timeout");

        NtClose(event);
    }

    TEST_END();
}
