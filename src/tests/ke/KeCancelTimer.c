#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeCancelTimer)
{
    TEST_BEGIN();

    KTIMER timer;
    LARGE_INTEGER due_time;

    // --- Cancel a pending timer ---
    KeInitializeTimerEx(&timer, NotificationTimer);
    due_time.QuadPart = -100000000; // 10 seconds (won't fire)
    KeSetTimer(&timer, due_time, NULL);

    BOOLEAN was_set = KeCancelTimer(&timer);
    GEN_CHECK(was_set, TRUE, "pending timer cancelled");
    GEN_CHECK(timer.Header.Inserted, FALSE, "no longer in timer queue");
    GEN_CHECK(timer.Header.SignalState, 0, "not signaled after cancel");

    // --- Cancel already cancelled (not in queue) ---
    was_set = KeCancelTimer(&timer);
    GEN_CHECK(was_set, FALSE, "already cancelled returns FALSE");

    // --- Cancel a timer that was never set ---
    KTIMER timer2;
    KeInitializeTimerEx(&timer2, NotificationTimer);
    was_set = KeCancelTimer(&timer2);
    GEN_CHECK(was_set, FALSE, "never-set timer returns FALSE");

    // --- Cancel and re-set ---
    KeSetTimer(&timer, due_time, NULL);
    KeCancelTimer(&timer);
    // Re-set with short due time
    due_time.QuadPart = -10000; // 1ms
    KeSetTimer(&timer, due_time, NULL);
    NTSTATUS status = KeWaitForSingleObject(&timer, Executive, KernelMode, FALSE, NULL);
    GEN_CHECK(status, STATUS_SUCCESS, "re-set timer fires");
    GEN_CHECK(timer.Header.SignalState, 1, "fired after re-set");

    // --- Cancel a signaled (already fired) timer ---
    // Timer already fired above, now cancel it
    was_set = KeCancelTimer(&timer);
    // Already fired timers are removed from queue, so returns FALSE
    GEN_CHECK(was_set, FALSE, "fired timer not in queue");

    // --- Cancel synchronization timer ---
    KTIMER sync_timer;
    KeInitializeTimerEx(&sync_timer, SynchronizationTimer);
    due_time.QuadPart = -100000000;
    KeSetTimer(&sync_timer, due_time, NULL);
    was_set = KeCancelTimer(&sync_timer);
    GEN_CHECK(was_set, TRUE, "sync timer cancelled");

    TEST_END();
}
