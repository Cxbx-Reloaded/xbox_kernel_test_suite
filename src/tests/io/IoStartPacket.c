#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"
#include "util/device_dummy.h"

#ifndef FILE_DEVICE_UNKNOWN
#define FILE_DEVICE_UNKNOWN 0x22
#endif

TEST_FUNC(IoStartPacket)
{
    TEST_BEGIN();

    PDEVICE_OBJECT device = NULL;
    NTSTATUS status;

    // Create a device for StartIo testing
    status = IoCreateDevice(&dummy_driver_object, 0, NULL, FILE_DEVICE_UNKNOWN, FALSE, &device);
    GEN_CHECK(status, STATUS_SUCCESS, "IoCreateDevice for StartPacket");
    if (NT_SUCCESS(status) && device) {
        PIRP irp = IoAllocateIrp(device->StackSize);
        GEN_CHECK(irp != NULL, TRUE, "IoAllocateIrp non-NULL");
        if (irp) {
            // IoStartPacket should set device->CurrentIrp and call StartIo
            IoStartPacket(device, irp, NULL);
            GEN_CHECK(device->CurrentIrp, irp, "CurrentIrp set by StartPacket");
            IoFreeIrp(irp);
            device->CurrentIrp = NULL;
        }
        IoDeleteDevice(device);
    }

    TEST_END();
}
