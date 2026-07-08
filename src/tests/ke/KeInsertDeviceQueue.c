#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeInsertDeviceQueue)
{
    TEST_BEGIN();

    KDEVICE_QUEUE queue;
    KDEVICE_QUEUE_ENTRY entry1, entry2, entry3;

    KeInitializeDeviceQueue(&queue);

    // --- First insert: not queued (device starts processing), sets busy ---
    BOOLEAN queued = KeInsertDeviceQueue(&queue, &entry1);
    GEN_CHECK(queued, FALSE, "first not queued (starts device)");
    GEN_CHECK(queue.Busy, TRUE, "now busy");

    // --- Subsequent inserts while busy: actually queued ---
    queued = KeInsertDeviceQueue(&queue, &entry2);
    GEN_CHECK(queued, TRUE, "second queued");
    GEN_CHECK(entry2.Inserted, TRUE, "entry2 Inserted flag");

    queued = KeInsertDeviceQueue(&queue, &entry3);
    GEN_CHECK(queued, TRUE, "third queued");

    // --- Queue is not empty (has entry2 and entry3) ---
    GEN_CHECK(IsListEmpty(&queue.DeviceListHead), FALSE, "queue not empty");

    // --- Remove in FIFO order ---
    PKDEVICE_QUEUE_ENTRY removed = KeRemoveDeviceQueue(&queue);
    GEN_CHECK(removed, &entry2, "FIFO: entry2 first");
    GEN_CHECK(entry2.Inserted, FALSE, "entry2 Inserted cleared");

    removed = KeRemoveDeviceQueue(&queue);
    GEN_CHECK(removed, &entry3, "FIFO: entry3 second");

    // --- Empty now: remove sets busy=FALSE ---
    removed = KeRemoveDeviceQueue(&queue);
    GEN_CHECK(removed == NULL, TRUE, "empty returns NULL");
    GEN_CHECK(queue.Busy, FALSE, "no longer busy");

    // --- Insert after drain: first insert starts new cycle ---
    queued = KeInsertDeviceQueue(&queue, &entry1);
    GEN_CHECK(queued, FALSE, "new cycle: not queued");
    GEN_CHECK(queue.Busy, TRUE, "busy again");

    // Clean up
    KeRemoveDeviceQueue(&queue);

    TEST_END();
}
