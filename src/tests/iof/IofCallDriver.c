#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/device_dummy.h"
#include "assertions/defines.h"

TEST_FUNC(IofCallDriver)
{
    TEST_BEGIN();

    // IofCallDriver dispatches an IRP to a device's driver.
    // Using our dummy device, it will call IoInvalidDeviceRequest.
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
        // Call driver - dummy returns STATUS_INVALID_DEVICE_REQUEST
        status = IofCallDriver(device, irp);
        GEN_CHECK(status, STATUS_INVALID_DEVICE_REQUEST, "IofCallDriver returns invalid device request");
        // IRP is completed by IoInvalidDeviceRequest, don't free
    }

    IoDeleteDevice(device);
    TEST_END();
}
