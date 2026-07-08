#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeInitializeEvent)
{
    TEST_BEGIN();

    // --- Notification event, initially not signaled ---
    KEVENT event;
    UCHAR canary_before[8];
    UCHAR canary_after[8];
    struct {
        UCHAR pre[8];
        KEVENT ev;
        UCHAR post[8];
    } guarded;

    // Set canary pattern to detect buffer overruns
    memset(guarded.pre, 0xAA, 8);
    memset(guarded.post, 0xBB, 8);

    KeInitializeEvent(&guarded.ev, NotificationEvent, FALSE);
    GEN_CHECK(guarded.ev.Header.Type, NotificationEvent, "notification type");
    GEN_CHECK(guarded.ev.Header.SignalState, 0, "not signaled initially");
    GEN_CHECK(guarded.ev.Header.Size, sizeof(KEVENT) / sizeof(ULONG), "header size");

    // Verify wait list is properly initialized (empty circular list)
    GEN_CHECK(guarded.ev.Header.WaitListHead.Flink, &guarded.ev.Header.WaitListHead, "waitlist flink self");
    GEN_CHECK(guarded.ev.Header.WaitListHead.Blink, &guarded.ev.Header.WaitListHead, "waitlist blink self");

    // Verify no buffer overrun
    GEN_CHECK(guarded.pre[0], 0xAA, "canary before intact [0]");
    GEN_CHECK(guarded.pre[7], 0xAA, "canary before intact [7]");
    GEN_CHECK(guarded.post[0], 0xBB, "canary after intact [0]");
    GEN_CHECK(guarded.post[7], 0xBB, "canary after intact [7]");

    // --- Notification event, initially signaled ---
    KeInitializeEvent(&event, NotificationEvent, TRUE);
    GEN_CHECK(event.Header.Type, NotificationEvent, "notification signaled type");
    GEN_CHECK(event.Header.SignalState, 1, "initially signaled");

    // --- Synchronization event, initially not signaled ---
    KeInitializeEvent(&event, SynchronizationEvent, FALSE);
    GEN_CHECK(event.Header.Type, SynchronizationEvent, "sync type");
    GEN_CHECK(event.Header.SignalState, 0, "sync not signaled");

    // --- Synchronization event, initially signaled ---
    KeInitializeEvent(&event, SynchronizationEvent, TRUE);
    GEN_CHECK(event.Header.Type, SynchronizationEvent, "sync signaled type");
    GEN_CHECK(event.Header.SignalState, 1, "sync initially signaled");

    // --- Re-initialization resets all state ---
    KeSetEvent(&event, IO_NO_INCREMENT, FALSE); // ensure signaled
    KeInitializeEvent(&event, NotificationEvent, FALSE); // re-init
    GEN_CHECK(event.Header.Type, NotificationEvent, "re-init changes type");
    GEN_CHECK(event.Header.SignalState, 0, "re-init resets signal");

    TEST_END();
}
