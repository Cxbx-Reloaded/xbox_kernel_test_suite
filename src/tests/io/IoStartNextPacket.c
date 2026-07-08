#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"
#include "util/device_dummy.h"

#ifndef FILE_DEVICE_UNKNOWN
#define FILE_DEVICE_UNKNOWN 0x22
#endif

TEST_FUNC(IoStartNextPacket)
{
    TEST_BEGIN();

    PDEVICE_OBJECT device = NULL;
    NTSTATUS status;

    // Create a device with StartIo routine
    status = IoCreateDevice(&dummy_driver_object, 0, NULL, FILE_DEVICE_UNKNOWN, FALSE, &device);
    GEN_CHECK(status, STATUS_SUCCESS, "IoCreateDevice for StartNextPacket");
    if (NT_SUCCESS(status) && device) {
        // With an empty device queue, IoStartNextPacket should just clear CurrentIrp
        device->CurrentIrp = NULL;
        IoStartNextPacket(device);
        GEN_CHECK(device->CurrentIrp, NULL, "CurrentIrp NULL after empty queue");

        IoDeleteDevice(device);
    }

    TEST_END();
}
