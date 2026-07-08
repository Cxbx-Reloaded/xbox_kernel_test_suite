#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeSetEvent)
{
    TEST_BEGIN();

    // --- Notification event: set returns previous state ---
    KEVENT event;
    KeInitializeEvent(&event, NotificationEvent, FALSE);

    LONG prev = KeSetEvent(&event, IO_NO_INCREMENT, FALSE);
    GEN_CHECK(prev, 0, "was not signaled");
    GEN_CHECK(event.Header.SignalState, 1, "now signaled");

    // Set again - returns 1 (already signaled), stays signaled
    prev = KeSetEvent(&event, IO_NO_INCREMENT, FALSE);
    GEN_CHECK(prev, 1, "already signaled");
    GEN_CHECK(event.Header.SignalState, 1, "still signaled");

    // --- Reset then set again ---
    KeResetEvent(&event);
    GEN_CHECK(event.Header.SignalState, 0, "reset to 0");
    prev = KeSetEvent(&event, IO_NO_INCREMENT, FALSE);
    GEN_CHECK(prev, 0, "was 0 after reset");
    GEN_CHECK(event.Header.SignalState, 1, "signaled again");

    // --- Synchronization event: set signals, wait auto-resets ---
    KEVENT sync_event;
    KeInitializeEvent(&sync_event, SynchronizationEvent, FALSE);

    prev = KeSetEvent(&sync_event, IO_NO_INCREMENT, FALSE);
    GEN_CHECK(prev, 0, "sync was not signaled");
    GEN_CHECK(sync_event.Header.SignalState, 1, "sync now signaled");

    // Wait consumes the signal (auto-reset)
    LARGE_INTEGER zero = {0};
    NTSTATUS status = KeWaitForSingleObject(&sync_event, Executive, KernelMode, FALSE, &zero);
    GEN_CHECK(status, STATUS_SUCCESS, "sync wait satisfied");
    GEN_CHECK(sync_event.Header.SignalState, 0, "sync auto-reset after wait");

    // --- Multiple set/reset cycles ---
    for (ULONG i = 0; i < 10; i++) {
        KeSetEvent(&event, IO_NO_INCREMENT, FALSE);
        GEN_CHECK(event.Header.SignalState, 1, "set in loop");
        KeResetEvent(&event);
        GEN_CHECK(event.Header.SignalState, 0, "reset in loop");
    }

    // --- Set with different increment values (doesn't affect event state) ---
    KeResetEvent(&event);
    prev = KeSetEvent(&event, 5, FALSE);
    GEN_CHECK(prev, 0, "set with increment=5");
    GEN_CHECK(event.Header.SignalState, 1, "signaled regardless of increment");

    TEST_END();
}
