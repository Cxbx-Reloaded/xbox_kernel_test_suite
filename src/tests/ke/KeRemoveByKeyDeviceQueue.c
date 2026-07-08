#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeRemoveByKeyDeviceQueue)
{
    TEST_BEGIN();

    KDEVICE_QUEUE queue;
    KDEVICE_QUEUE_ENTRY entry1, entry2, entry3;

    KeInitializeDeviceQueue(&queue);
    KeInsertByKeyDeviceQueue(&queue, &entry1, 0); // starts device
    KeInsertByKeyDeviceQueue(&queue, &entry2, 100);
    KeInsertByKeyDeviceQueue(&queue, &entry3, 200);

    // Remove by key - should find entry nearest to sort key
    PKDEVICE_QUEUE_ENTRY removed = KeRemoveByKeyDeviceQueue(&queue, 150);
    GEN_CHECK(removed != NULL, TRUE, "removed entry");

    TEST_END();
}
