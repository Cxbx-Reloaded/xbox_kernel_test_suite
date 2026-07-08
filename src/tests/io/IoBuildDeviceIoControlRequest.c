#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "util/device_dummy.h"
#include "assertions/defines.h"

TEST_FUNC(IoBuildDeviceIoControlRequest)
{
    TEST_BEGIN();

    PDEVICE_OBJECT device = NULL;
    NTSTATUS status = IoCreateDevice(&dummy_driver_object, 0, NULL, FILE_DEVICE_CD_ROM, FALSE, &device);
    if (!NT_SUCCESS(status)) {
        TEST_SKIP("IoCreateDevice failed");
        TEST_END();
        return;
    }

    UCHAR input_buf[64];
    UCHAR output_buf[64];
    memset(input_buf, 0xAA, sizeof(input_buf));
    memset(output_buf, 0, sizeof(output_buf));

    KEVENT event;
    KeInitializeEvent(&event, NotificationEvent, FALSE);
    IO_STATUS_BLOCK iosb;

    // Build a device IO control IRP
    PIRP irp = IoBuildDeviceIoControlRequest(
        0x12345678, device, input_buf, sizeof(input_buf),
        output_buf, sizeof(output_buf), FALSE, &event, &iosb);
    GEN_CHECK(irp != NULL, TRUE, "IoBuildDeviceIoCtl non-NULL");
    if (irp) {
        GEN_CHECK(irp->StackCount, 1, "StackCount == 1");
        IoFreeIrp(irp);
    }

    // Internal device control
    irp = IoBuildDeviceIoControlRequest(
        0x12345678, device, NULL, 0, NULL, 0, TRUE, &event, &iosb);
    GEN_CHECK(irp != NULL, TRUE, "IoBuildDeviceIoCtl internal non-NULL");
    if (irp) {
        IoFreeIrp(irp);
    }

    IoDeleteDevice(device);
    TEST_END();
}
