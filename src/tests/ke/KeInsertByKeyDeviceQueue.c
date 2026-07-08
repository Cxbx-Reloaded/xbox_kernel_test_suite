#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeInsertByKeyDeviceQueue)
{
    TEST_BEGIN();

    KDEVICE_QUEUE queue;
    KDEVICE_QUEUE_ENTRY entry1, entry2, entry3;

    KeInitializeDeviceQueue(&queue);

    // First insert (not busy) - starts device
    BOOLEAN queued = KeInsertByKeyDeviceQueue(&queue, &entry1, 100);
    GEN_CHECK(queued, FALSE, "first not queued");

    // Insert by key - should be sorted
    KeInsertByKeyDeviceQueue(&queue, &entry2, 50);
    KeInsertByKeyDeviceQueue(&queue, &entry3, 75);

    // Remove should get lowest key first (entry2, key=50)
    PKDEVICE_QUEUE_ENTRY removed = KeRemoveByKeyDeviceQueue(&queue, 0);
    GEN_CHECK(removed, &entry2, "lowest key first");

    // Next should be entry3 (key=75)
    removed = KeRemoveByKeyDeviceQueue(&queue, 0);
    GEN_CHECK(removed, &entry3, "next key");

    TEST_END();
}
