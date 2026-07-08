#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "util/device_dummy.h"
#include "assertions/defines.h"

TEST_FUNC(IoBuildSynchronousFsdRequest)
{
    TEST_BEGIN();

    PDEVICE_OBJECT device = NULL;
    NTSTATUS status = IoCreateDevice(&dummy_driver_object, 0, NULL, FILE_DEVICE_CD_ROM, FALSE, &device);
    if (!NT_SUCCESS(status)) {
        TEST_SKIP("IoCreateDevice failed");
        TEST_END();
        return;
    }

    UCHAR buffer[512];
    KEVENT event;
    IO_STATUS_BLOCK iosb;
    LARGE_INTEGER offset;
    offset.QuadPart = 0;

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    // Build a synchronous read IRP
    PIRP irp = IoBuildSynchronousFsdRequest(IRP_MJ_READ, device, buffer, sizeof(buffer), &offset, &event, &iosb);
    GEN_CHECK(irp != NULL, TRUE, "IoBuildSyncFsd READ non-NULL");
    if (irp) {
        GEN_CHECK(irp->StackCount, 1, "StackCount == 1");
        IoFreeIrp(irp);
    }

    IoDeleteDevice(device);
    TEST_END();
}
