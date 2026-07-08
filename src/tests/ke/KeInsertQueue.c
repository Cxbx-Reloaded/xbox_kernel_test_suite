#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeInsertQueue)
{
    TEST_BEGIN();

    KQUEUE queue;
    LIST_ENTRY entry1, entry2, entry3;
    LARGE_INTEGER zero = {0};

    KeInitializeQueue(&queue, 1);

    // --- First insert: returns 0 (was empty) ---
    LONG prev = KeInsertQueue(&queue, &entry1);
    GEN_CHECK(prev, 0, "first insert prev=0");
    GEN_CHECK(queue.Header.SignalState, 1, "count=1");

    // --- Second insert: returns 1 ---
    prev = KeInsertQueue(&queue, &entry2);
    GEN_CHECK(prev, 1, "second insert prev=1");
    GEN_CHECK(queue.Header.SignalState, 2, "count=2");

    // --- Third insert ---
    prev = KeInsertQueue(&queue, &entry3);
    GEN_CHECK(prev, 2, "third insert prev=2");
    GEN_CHECK(queue.Header.SignalState, 3, "count=3");

    // --- Remove all in FIFO order ---
    PLIST_ENTRY removed = KeRemoveQueue(&queue, KernelMode, &zero);
    GEN_CHECK(removed, &entry1, "FIFO: entry1");

    removed = KeRemoveQueue(&queue, KernelMode, &zero);
    GEN_CHECK(removed, &entry2, "FIFO: entry2");

    removed = KeRemoveQueue(&queue, KernelMode, &zero);
    GEN_CHECK(removed, &entry3, "FIFO: entry3");

    // --- Insert after drain ---
    prev = KeInsertQueue(&queue, &entry1);
    GEN_CHECK(prev, 0, "insert after drain prev=0");
    KeRemoveQueue(&queue, KernelMode, &zero);

    // --- Interleaved insert/remove ---
    KeInsertQueue(&queue, &entry1);
    removed = KeRemoveQueue(&queue, KernelMode, &zero);
    GEN_CHECK(removed, &entry1, "interleave 1");

    KeInsertQueue(&queue, &entry2);
    KeInsertQueue(&queue, &entry3);
    removed = KeRemoveQueue(&queue, KernelMode, &zero);
    GEN_CHECK(removed, &entry2, "interleave 2");
    removed = KeRemoveQueue(&queue, KernelMode, &zero);
    GEN_CHECK(removed, &entry3, "interleave 3");

    TEST_END();
}
