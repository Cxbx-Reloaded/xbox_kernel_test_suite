#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeRemoveDeviceQueue)
{
    TEST_BEGIN();

    KDEVICE_QUEUE queue;
    KDEVICE_QUEUE_ENTRY entry1, entry2, entry3;

    KeInitializeDeviceQueue(&queue);
    KeInsertDeviceQueue(&queue, &entry1); // starts device, busy=TRUE
    KeInsertDeviceQueue(&queue, &entry2); // queued
    KeInsertDeviceQueue(&queue, &entry3); // queued

    // --- Remove returns queued entries in FIFO order ---
    PKDEVICE_QUEUE_ENTRY removed = KeRemoveDeviceQueue(&queue);
    GEN_CHECK(removed, &entry2, "first remove: entry2");

    removed = KeRemoveDeviceQueue(&queue);
    GEN_CHECK(removed, &entry3, "second remove: entry3");

    // --- Remove from empty queue sets busy=FALSE, returns NULL ---
    removed = KeRemoveDeviceQueue(&queue);
    GEN_CHECK(removed == NULL, TRUE, "empty returns NULL");
    GEN_CHECK(queue.Busy, FALSE, "busy cleared");

    // --- Remove from non-busy queue ---
    // After busy is FALSE, calling Remove should return NULL
    removed = KeRemoveDeviceQueue(&queue);
    GEN_CHECK(removed == NULL, TRUE, "non-busy returns NULL");

    // --- Pattern: insert/remove/insert/remove cycle ---
    KeInsertDeviceQueue(&queue, &entry1); // starts device
    KeInsertDeviceQueue(&queue, &entry2); // queued

    removed = KeRemoveDeviceQueue(&queue);
    GEN_CHECK(removed, &entry2, "cycle remove 1");

    KeInsertDeviceQueue(&queue, &entry3); // queued (device still busy)
    removed = KeRemoveDeviceQueue(&queue);
    GEN_CHECK(removed, &entry3, "cycle remove 2");

    // Drain
    removed = KeRemoveDeviceQueue(&queue);
    GEN_CHECK(removed == NULL, TRUE, "cycle drain");
    GEN_CHECK(queue.Busy, FALSE, "cycle done");

    TEST_END();
}
