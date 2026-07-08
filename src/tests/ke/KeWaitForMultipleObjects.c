#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeWaitForMultipleObjects)
{
    TEST_BEGIN();

    KEVENT events[3];
    PVOID wait_objects[3];
    KWAIT_BLOCK wait_blocks[3];
    LARGE_INTEGER timeout;
    timeout.QuadPart = 0;

    // --- WaitAll: all signaled = immediate success ---
    for (int i = 0; i < 3; i++) {
        KeInitializeEvent(&events[i], NotificationEvent, TRUE);
        wait_objects[i] = &events[i];
    }

    NTSTATUS status = KeWaitForMultipleObjects(
        3, wait_objects, WaitAll, Executive, KernelMode, FALSE, NULL, wait_blocks
    );
    GEN_CHECK(status, STATUS_SUCCESS, "WaitAll all signaled");

    // --- WaitAny: first signaled = returns STATUS_WAIT_0 ---
    status = KeWaitForMultipleObjects(
        3, wait_objects, WaitAny, Executive, KernelMode, FALSE, &timeout, wait_blocks
    );
    GEN_CHECK(status, STATUS_WAIT_0, "WaitAny returns WAIT_0");

    // --- WaitAny: only second signaled = returns STATUS_WAIT_0 + 1 ---
    KeResetEvent(&events[0]);
    KeResetEvent(&events[2]);
    status = KeWaitForMultipleObjects(
        3, wait_objects, WaitAny, Executive, KernelMode, FALSE, &timeout, wait_blocks
    );
    GEN_CHECK(status, STATUS_WAIT_0 + 1, "WaitAny second = WAIT_1");

    // --- WaitAny: only third signaled ---
    KeResetEvent(&events[1]);
    KeSetEvent(&events[2], IO_NO_INCREMENT, FALSE);
    status = KeWaitForMultipleObjects(
        3, wait_objects, WaitAny, Executive, KernelMode, FALSE, &timeout, wait_blocks
    );
    GEN_CHECK(status, STATUS_WAIT_0 + 2, "WaitAny third = WAIT_2");

    // --- WaitAll: one unsignaled = timeout ---
    KeSetEvent(&events[0], IO_NO_INCREMENT, FALSE);
    // events[1] is unsignaled, events[2] is signaled
    status = KeWaitForMultipleObjects(
        3, wait_objects, WaitAll, Executive, KernelMode, FALSE, &timeout, wait_blocks
    );
    GEN_CHECK(status, STATUS_TIMEOUT, "WaitAll one unsignaled");

    // --- WaitAny: none signaled = timeout ---
    KeResetEvent(&events[0]);
    KeResetEvent(&events[2]);
    status = KeWaitForMultipleObjects(
        3, wait_objects, WaitAny, Executive, KernelMode, FALSE, &timeout, wait_blocks
    );
    GEN_CHECK(status, STATUS_TIMEOUT, "WaitAny none signaled");

    // --- WaitAll with synchronization events: auto-resets all ---
    KEVENT sync_events[2];
    PVOID sync_objects[2];
    KWAIT_BLOCK sync_blocks[2];
    KeInitializeEvent(&sync_events[0], SynchronizationEvent, TRUE);
    KeInitializeEvent(&sync_events[1], SynchronizationEvent, TRUE);
    sync_objects[0] = &sync_events[0];
    sync_objects[1] = &sync_events[1];

    status = KeWaitForMultipleObjects(
        2, sync_objects, WaitAll, Executive, KernelMode, FALSE, &timeout, sync_blocks
    );
    GEN_CHECK(status, STATUS_SUCCESS, "sync WaitAll satisfied");
    GEN_CHECK(sync_events[0].Header.SignalState, 0, "sync[0] auto-reset");
    GEN_CHECK(sync_events[1].Header.SignalState, 0, "sync[1] auto-reset");

    // --- Single object in array (count=1) ---
    KeSetEvent(&events[0], IO_NO_INCREMENT, FALSE);
    status = KeWaitForMultipleObjects(
        1, wait_objects, WaitAny, Executive, KernelMode, FALSE, &timeout, wait_blocks
    );
    GEN_CHECK(status, STATUS_WAIT_0, "single object WaitAny");

    // --- Mixed object types: event + semaphore ---
    KSEMAPHORE sem;
    KeInitializeSemaphore(&sem, 1, 5);
    KeSetEvent(&events[0], IO_NO_INCREMENT, FALSE);
    PVOID mixed[2] = { &events[0], &sem };
    KWAIT_BLOCK mixed_blocks[2];
    status = KeWaitForMultipleObjects(
        2, mixed, WaitAll, Executive, KernelMode, FALSE, &timeout, mixed_blocks
    );
    GEN_CHECK(status, STATUS_SUCCESS, "mixed event+sem WaitAll");

    TEST_END();
}
