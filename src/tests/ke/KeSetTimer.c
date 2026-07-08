#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeSetTimer)
{
    TEST_BEGIN();

    KTIMER timer;
    LARGE_INTEGER due_time;

    // --- Set and wait for a short timer ---
    KeInitializeTimerEx(&timer, NotificationTimer);
    due_time.QuadPart = -10000; // 1ms relative

    BOOLEAN was_set = KeSetTimer(&timer, due_time, NULL);
    GEN_CHECK(was_set, FALSE, "timer was not previously set");
    GEN_CHECK(timer.Header.Inserted, TRUE, "timer inserted");

    NTSTATUS status = KeWaitForSingleObject(&timer, Executive, KernelMode, FALSE, NULL);
    GEN_CHECK(status, STATUS_SUCCESS, "timer fired");
    GEN_CHECK(timer.Header.SignalState, 1, "timer signaled after fire");

    // --- Set an already-signaled timer (re-arm) ---
    due_time.QuadPart = -10000;
    was_set = KeSetTimer(&timer, due_time, NULL);
    // Previous state was signaled, so was_set returns TRUE
    GEN_CHECK(was_set, TRUE, "timer was previously signaled");
    // Timer should be re-armed (not signaled until fires)
    GEN_CHECK(timer.Header.SignalState, 0, "re-armed timer not signaled");

    status = KeWaitForSingleObject(&timer, Executive, KernelMode, FALSE, NULL);
    GEN_CHECK(status, STATUS_SUCCESS, "re-armed timer fired");

    // --- Cancel before fire ---
    KeInitializeTimerEx(&timer, NotificationTimer);
    due_time.QuadPart = -10000000; // 1 second (won't fire in time)
    KeSetTimer(&timer, due_time, NULL);
    BOOLEAN was_inserted = KeCancelTimer(&timer);
    GEN_CHECK(was_inserted, TRUE, "cancel returns TRUE (was pending)");
    GEN_CHECK(timer.Header.SignalState, 0, "cancelled timer not signaled");

    // Poll: should timeout since timer was cancelled
    LARGE_INTEGER zero_timeout;
    zero_timeout.QuadPart = 0;
    status = KeWaitForSingleObject(&timer, Executive, KernelMode, FALSE, &zero_timeout);
    GEN_CHECK(status, STATUS_TIMEOUT, "cancelled timer not satisfied");

    // --- Synchronization timer auto-resets on wait ---
    KTIMER sync_timer;
    KeInitializeTimerEx(&sync_timer, SynchronizationTimer);
    due_time.QuadPart = -10000;
    KeSetTimer(&sync_timer, due_time, NULL);
    status = KeWaitForSingleObject(&sync_timer, Executive, KernelMode, FALSE, NULL);
    GEN_CHECK(status, STATUS_SUCCESS, "sync timer fired");
    GEN_CHECK(sync_timer.Header.SignalState, 0, "sync timer auto-reset");

    // --- Timer with very short due time (immediate) ---
    KeInitializeTimerEx(&timer, NotificationTimer);
    due_time.QuadPart = -1; // Practically immediate
    KeSetTimer(&timer, due_time, NULL);
    // Short delay then check
    LARGE_INTEGER short_wait;
    short_wait.QuadPart = -50000; // 5ms
    status = KeWaitForSingleObject(&timer, Executive, KernelMode, FALSE, &short_wait);
    GEN_CHECK(status, STATUS_SUCCESS, "immediate timer fires quickly");

    TEST_END();
}
