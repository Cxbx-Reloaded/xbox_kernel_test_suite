#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeResetEvent)
{
    TEST_BEGIN();

    // --- Reset signaled notification event ---
    KEVENT event;
    KeInitializeEvent(&event, NotificationEvent, TRUE);

    LONG prev = KeResetEvent(&event);
    GEN_CHECK(prev, 1, "was signaled");
    GEN_CHECK(event.Header.SignalState, 0, "now not signaled");

    // --- Reset already unsignaled event ---
    prev = KeResetEvent(&event);
    GEN_CHECK(prev, 0, "was not signaled");
    GEN_CHECK(event.Header.SignalState, 0, "still not signaled");

    // --- Reset after KeSetEvent ---
    KeSetEvent(&event, IO_NO_INCREMENT, FALSE);
    GEN_CHECK(event.Header.SignalState, 1, "set before reset");
    prev = KeResetEvent(&event);
    GEN_CHECK(prev, 1, "was signaled after set");
    GEN_CHECK(event.Header.SignalState, 0, "reset after set");

    // --- Reset synchronization event ---
    KEVENT sync_event;
    KeInitializeEvent(&sync_event, SynchronizationEvent, TRUE);
    prev = KeResetEvent(&sync_event);
    GEN_CHECK(prev, 1, "sync was signaled");
    GEN_CHECK(sync_event.Header.SignalState, 0, "sync now not signaled");

    // --- Multiple reset cycles ---
    for (ULONG i = 0; i < 5; i++) {
        KeSetEvent(&event, IO_NO_INCREMENT, FALSE);
        prev = KeResetEvent(&event);
        GEN_CHECK(prev, 1, "cycle reset returns 1");
    }
    GEN_CHECK(event.Header.SignalState, 0, "final state after cycles");

    TEST_END();
}
