#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeInitializeTimerEx)
{
    TEST_BEGIN();

    KTIMER timer;

    // --- Notification timer ---
    memset(&timer, 0xCC, sizeof(timer));
    KeInitializeTimerEx(&timer, NotificationTimer);
    GEN_CHECK(timer.Header.Type, TimerNotificationObject, "notification type");
    GEN_CHECK(timer.Header.SignalState, 0, "not signaled");
    GEN_CHECK(timer.Header.Inserted, FALSE, "not inserted in timer queue");

    // Wait list should be initialized (empty circular)
    GEN_CHECK(timer.Header.WaitListHead.Flink, &timer.Header.WaitListHead, "waitlist flink self");
    GEN_CHECK(timer.Header.WaitListHead.Blink, &timer.Header.WaitListHead, "waitlist blink self");

    // --- Synchronization timer ---
    memset(&timer, 0xCC, sizeof(timer));
    KeInitializeTimerEx(&timer, SynchronizationTimer);
    GEN_CHECK(timer.Header.Type, TimerSynchronizationObject, "sync type");
    GEN_CHECK(timer.Header.SignalState, 0, "sync not signaled");
    GEN_CHECK(timer.Header.Inserted, FALSE, "sync not inserted");

    // --- DueTime should be 0 after init ---
    GEN_CHECK(timer.DueTime.QuadPart, 0, "due time 0");

    // --- Re-initialization (cancel + reinit) ---
    // Set a timer, then reinitialize
    LARGE_INTEGER due;
    due.QuadPart = -100000000; // 10 seconds (won't fire)
    KeSetTimer(&timer, due, NULL);
    GEN_CHECK(timer.Header.Inserted, TRUE, "timer inserted after set");

    KeCancelTimer(&timer);
    KeInitializeTimerEx(&timer, NotificationTimer);
    GEN_CHECK(timer.Header.Type, TimerNotificationObject, "re-init type");
    GEN_CHECK(timer.Header.SignalState, 0, "re-init not signaled");
    GEN_CHECK(timer.Header.Inserted, FALSE, "re-init not inserted");

    // --- Two timers are independent ---
    KTIMER timer2;
    KeInitializeTimerEx(&timer, NotificationTimer);
    KeInitializeTimerEx(&timer2, SynchronizationTimer);
    GEN_CHECK(timer.Header.Type, TimerNotificationObject, "timer1 type");
    GEN_CHECK(timer2.Header.Type, TimerSynchronizationObject, "timer2 type");

    TEST_END();
}
