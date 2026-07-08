#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeInitializeQueue)
{
    TEST_BEGIN();

    KQUEUE queue;

    // --- Basic initialization ---
    memset(&queue, 0xCC, sizeof(queue));
    KeInitializeQueue(&queue, 0); // 0 = number of processors

    GEN_CHECK(queue.Header.SignalState, 0, "no entries");
    GEN_CHECK(queue.Header.Type, QueueObject, "type is QueueObject");
    GEN_CHECK(IsListEmpty(&queue.EntryListHead), TRUE, "entry list empty");
    GEN_CHECK(queue.CurrentCount, 0, "current count 0");
    GEN_CHECK(queue.MaximumCount != 0, TRUE, "max count set (>0)");

    // --- Explicit maximum count ---
    KeInitializeQueue(&queue, 4);
    GEN_CHECK(queue.MaximumCount, 4, "explicit max=4");
    GEN_CHECK(queue.Header.SignalState, 0, "still empty");

    // --- Insert after init works ---
    LIST_ENTRY entry;
    LONG prev = KeInsertQueue(&queue, &entry);
    GEN_CHECK(prev, 0, "insert into fresh queue");
    GEN_CHECK(queue.Header.SignalState, 1, "one entry");

    // Clean up
    LARGE_INTEGER zero = {0};
    KeRemoveQueue(&queue, KernelMode, &zero);

    // --- Two queues are independent ---
    KQUEUE queue2;
    KeInitializeQueue(&queue2, 2);
    KeInsertQueue(&queue, &entry);
    GEN_CHECK(queue.Header.SignalState, 1, "queue1 has entry");
    GEN_CHECK(queue2.Header.SignalState, 0, "queue2 empty");
    KeRemoveQueue(&queue, KernelMode, &zero);

    TEST_END();
}
