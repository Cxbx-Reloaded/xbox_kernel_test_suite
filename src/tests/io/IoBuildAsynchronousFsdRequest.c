#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "util/device_dummy.h"
#include "assertions/defines.h"

TEST_FUNC(IoBuildAsynchronousFsdRequest)
{
    TEST_BEGIN();

    // Need a device object for IRP building
    PDEVICE_OBJECT device = NULL;
    NTSTATUS status = IoCreateDevice(&dummy_driver_object, 0, NULL, FILE_DEVICE_CD_ROM, FALSE, &device);
    if (!NT_SUCCESS(status)) {
        TEST_SKIP("IoCreateDevice failed");
        TEST_END();
        return;
    }

    // Build an async read request
    UCHAR buffer[512];
    IO_STATUS_BLOCK iosb;
    LARGE_INTEGER offset;
    offset.QuadPart = 0;

    PIRP irp = IoBuildAsynchronousFsdRequest(IRP_MJ_READ, device, buffer, sizeof(buffer), &offset, &iosb);
    GEN_CHECK(irp != NULL, TRUE, "IoBuildAsyncFsd READ non-NULL");
    if (irp) {
        GEN_CHECK(irp->StackCount, 1, "StackCount == 1");
        IoFreeIrp(irp);
    }

    // Build an async write request
    irp = IoBuildAsynchronousFsdRequest(IRP_MJ_WRITE, device, buffer, sizeof(buffer), &offset, &iosb);
    GEN_CHECK(irp != NULL, TRUE, "IoBuildAsyncFsd WRITE non-NULL");
    if (irp) {
        IoFreeIrp(irp);
    }

    IoDeleteDevice(device);
    TEST_END();
}
