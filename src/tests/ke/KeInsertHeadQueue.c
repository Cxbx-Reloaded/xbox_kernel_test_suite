#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeInsertHeadQueue)
{
    TEST_BEGIN();

    KQUEUE queue;
    LIST_ENTRY entry1, entry2;

    KeInitializeQueue(&queue, 1);

    KeInsertQueue(&queue, &entry1);
    KeInsertHeadQueue(&queue, &entry2);

    // entry2 should come out first (it was inserted at head)
    LARGE_INTEGER timeout;
    timeout.QuadPart = 0;
    PLIST_ENTRY removed = KeRemoveQueue(&queue, KernelMode, &timeout);
    GEN_CHECK(removed, &entry2, "head entry removed first");

    // Clean up
    KeRemoveQueue(&queue, KernelMode, &timeout);

    TEST_END();
}
