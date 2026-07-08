#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeInitializeDeviceQueue)
{
    TEST_BEGIN();

    KDEVICE_QUEUE queue;

    // --- Basic initialization ---
    memset(&queue, 0xCC, sizeof(queue));
    KeInitializeDeviceQueue(&queue);

    GEN_CHECK(queue.Busy, FALSE, "not busy");
    GEN_CHECK(IsListEmpty(&queue.DeviceListHead), TRUE, "list empty");
    GEN_CHECK(queue.DeviceListHead.Flink, &queue.DeviceListHead, "flink self");
    GEN_CHECK(queue.DeviceListHead.Blink, &queue.DeviceListHead, "blink self");

    // --- Two queues are independent ---
    KDEVICE_QUEUE queue2;
    KeInitializeDeviceQueue(&queue2);
    KDEVICE_QUEUE_ENTRY entry;
    KeInsertDeviceQueue(&queue, &entry); // makes queue busy
    GEN_CHECK(queue.Busy, TRUE, "queue1 busy");
    GEN_CHECK(queue2.Busy, FALSE, "queue2 still not busy");

    // Clean up
    KeRemoveDeviceQueue(&queue);

    // --- Re-initialization resets state ---
    KeInsertDeviceQueue(&queue, &entry); // busy again
    KeInitializeDeviceQueue(&queue); // re-init
    GEN_CHECK(queue.Busy, FALSE, "re-init resets busy");
    GEN_CHECK(IsListEmpty(&queue.DeviceListHead), TRUE, "re-init empties list");

    TEST_END();
}
