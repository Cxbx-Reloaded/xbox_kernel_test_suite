#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KePulseEvent)
{
    TEST_BEGIN();

    // --- Pulse unsignaled notification event ---
    KEVENT event;
    KeInitializeEvent(&event, NotificationEvent, FALSE);

    LONG prev = KePulseEvent(&event, IO_NO_INCREMENT, FALSE);
    GEN_CHECK(prev, 0, "was not signaled");
    // With no waiters, pulse sets then immediately resets
    GEN_CHECK(event.Header.SignalState, 0, "back to unsignaled");

    // --- Pulse already-signaled notification event ---
    KeSetEvent(&event, IO_NO_INCREMENT, FALSE);
    prev = KePulseEvent(&event, IO_NO_INCREMENT, FALSE);
    GEN_CHECK(prev, 1, "was signaled");
    GEN_CHECK(event.Header.SignalState, 0, "reset after pulse");

    // --- Pulse synchronization event ---
    KEVENT sync_event;
    KeInitializeEvent(&sync_event, SynchronizationEvent, FALSE);

    prev = KePulseEvent(&sync_event, IO_NO_INCREMENT, FALSE);
    GEN_CHECK(prev, 0, "sync was not signaled");
    GEN_CHECK(sync_event.Header.SignalState, 0, "sync remains unsignaled");

    // --- Pulse already-signaled synchronization event ---
    KeSetEvent(&sync_event, IO_NO_INCREMENT, FALSE);
    prev = KePulseEvent(&sync_event, IO_NO_INCREMENT, FALSE);
    GEN_CHECK(prev, 1, "sync was signaled");
    GEN_CHECK(sync_event.Header.SignalState, 0, "sync pulsed back to 0");

    // --- Multiple pulses don't accumulate ---
    KeInitializeEvent(&event, NotificationEvent, FALSE);
    KePulseEvent(&event, IO_NO_INCREMENT, FALSE);
    KePulseEvent(&event, IO_NO_INCREMENT, FALSE);
    KePulseEvent(&event, IO_NO_INCREMENT, FALSE);
    GEN_CHECK(event.Header.SignalState, 0, "multiple pulses, still 0");

    // --- Pulse returns correct previous state each time ---
    KeSetEvent(&event, IO_NO_INCREMENT, FALSE);
    prev = KePulseEvent(&event, IO_NO_INCREMENT, FALSE);
    GEN_CHECK(prev, 1, "first pulse of signaled");
    prev = KePulseEvent(&event, IO_NO_INCREMENT, FALSE);
    GEN_CHECK(prev, 0, "second pulse (was just pulsed)");

    // Note: Pulse is primarily useful when there are waiters - it releases
    // them and immediately resets. Without waiters it's effectively a no-op.
    // Testing with waiters requires a helper thread (covered in
    // KeWaitForSingleObject multi-thread test).

    TEST_END();
}
