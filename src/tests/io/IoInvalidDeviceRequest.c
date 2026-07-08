#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/device_dummy.h"
#include "assertions/defines.h"

TEST_FUNC(IoInvalidDeviceRequest)
{
    TEST_BEGIN();

    // IoInvalidDeviceRequest sets the IRP status to STATUS_INVALID_DEVICE_REQUEST
    // and completes it. We need a device and an IRP.
    PDEVICE_OBJECT device = NULL;
    NTSTATUS status = IoCreateDevice(&dummy_driver_object, 0, NULL, FILE_DEVICE_CD_ROM, FALSE, &device);
    if (!NT_SUCCESS(status)) {
        TEST_SKIP("IoCreateDevice failed");
        TEST_END();
        return;
    }

    PIRP irp = IoAllocateIrp(1);
    GEN_CHECK(irp != NULL, TRUE, "IRP allocated");
    if (irp) {
        // Call IoInvalidDeviceRequest - it completes the IRP
        NTSTATUS result = IoInvalidDeviceRequest(device, irp);
        GEN_CHECK(result, STATUS_INVALID_DEVICE_REQUEST, "returns STATUS_INVALID_DEVICE_REQUEST");
        // IRP is completed/freed by IoInvalidDeviceRequest, don't free again
    }

    IoDeleteDevice(device);
    TEST_END();
}
