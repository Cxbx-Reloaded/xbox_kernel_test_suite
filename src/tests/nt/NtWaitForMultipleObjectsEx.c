#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtWaitForMultipleObjectsEx)
{
    TEST_BEGIN();

    HANDLE events[3];
    NTSTATUS status;
    LARGE_INTEGER timeout;
    timeout.QuadPart = 0;

    // Create 3 events: first two signaled, third non-signaled
    status = NtCreateEvent(&events[0], NULL, NotificationEvent, TRUE);
    GEN_CHECK(status, STATUS_SUCCESS, "create event 0");
    status = NtCreateEvent(&events[1], NULL, NotificationEvent, TRUE);
    GEN_CHECK(status, STATUS_SUCCESS, "create event 1");
    status = NtCreateEvent(&events[2], NULL, NotificationEvent, FALSE);
    GEN_CHECK(status, STATUS_SUCCESS, "create event 2");

    // --- WaitAny: should return index of first signaled (0) ---
    status = NtWaitForMultipleObjectsEx(3, events, WaitAny, KernelMode, FALSE, &timeout);
    GEN_CHECK(status, STATUS_WAIT_0, "WaitAny: first signaled");

    // --- WaitAll: should timeout (event[2] not signaled) ---
    status = NtWaitForMultipleObjectsEx(3, events, WaitAll, KernelMode, FALSE, &timeout);
    GEN_CHECK(status, STATUS_TIMEOUT, "WaitAll: timeout");

    // Signal event[2], now WaitAll should succeed
    NtSetEvent(events[2], NULL);
    status = NtWaitForMultipleObjectsEx(3, events, WaitAll, KernelMode, FALSE, &timeout);
    GEN_CHECK(status, STATUS_WAIT_0, "WaitAll: all signaled");

    // --- Single object via WaitAny ---
    status = NtWaitForMultipleObjectsEx(1, events, WaitAny, KernelMode, FALSE, &timeout);
    GEN_CHECK(status, STATUS_WAIT_0, "single object WaitAny");

    for (int i = 0; i < 3; i++) NtClose(events[i]);

    TEST_END();
}
