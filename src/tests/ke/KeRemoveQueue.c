#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeRemoveQueue)
{
    TEST_BEGIN();

    KQUEUE queue;
    LIST_ENTRY entry1, entry2, entry3;
    LARGE_INTEGER timeout;
    timeout.QuadPart = 0;

    KeInitializeQueue(&queue, 1);

    // --- Remove from empty queue: returns STATUS_TIMEOUT ---
    PLIST_ENTRY removed = KeRemoveQueue(&queue, KernelMode, &timeout);
    GEN_CHECK((NTSTATUS)(ULONG_PTR)removed, STATUS_TIMEOUT, "empty timeout");

    // --- Insert 1, remove 1 ---
    KeInsertQueue(&queue, &entry1);
    removed = KeRemoveQueue(&queue, KernelMode, &timeout);
    GEN_CHECK(removed, &entry1, "single entry");
    GEN_CHECK(queue.Header.SignalState, 0, "count back to 0");

    // --- Insert 3, remove in FIFO order ---
    KeInsertQueue(&queue, &entry1);
    KeInsertQueue(&queue, &entry2);
    KeInsertQueue(&queue, &entry3);

    removed = KeRemoveQueue(&queue, KernelMode, &timeout);
    GEN_CHECK(removed, &entry1, "FIFO: entry1");
    removed = KeRemoveQueue(&queue, KernelMode, &timeout);
    GEN_CHECK(removed, &entry2, "FIFO: entry2");
    removed = KeRemoveQueue(&queue, KernelMode, &timeout);
    GEN_CHECK(removed, &entry3, "FIFO: entry3");

    // --- Empty again ---
    removed = KeRemoveQueue(&queue, KernelMode, &timeout);
    GEN_CHECK((NTSTATUS)(ULONG_PTR)removed, STATUS_TIMEOUT, "empty after drain");

    // --- Short timeout on empty queue ---
    LARGE_INTEGER short_timeout;
    short_timeout.QuadPart = -10000; // 1ms
    LARGE_INTEGER before, after;
    KeQuerySystemTime(&before);
    removed = KeRemoveQueue(&queue, KernelMode, &short_timeout);
    KeQuerySystemTime(&after);
    GEN_CHECK((NTSTATUS)(ULONG_PTR)removed, STATUS_TIMEOUT, "1ms timeout");
    LONGLONG elapsed = after.QuadPart - before.QuadPart;
    GEN_CHECK(elapsed >= 5000, TRUE, "some time elapsed (>0.5ms)");

    // --- NULL timeout means infinite wait (tested via pre-inserted entry) ---
    KeInsertQueue(&queue, &entry1);
    removed = KeRemoveQueue(&queue, KernelMode, NULL);
    GEN_CHECK(removed, &entry1, "NULL timeout with entry");

    TEST_END();
}
