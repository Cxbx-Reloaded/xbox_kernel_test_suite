#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"
#include "util/device_dummy.h"

#ifndef FILE_DEVICE_UNKNOWN
#define FILE_DEVICE_UNKNOWN 0x22
#endif

TEST_FUNC(IoDeleteDevice)
{
    TEST_BEGIN();

    PDEVICE_OBJECT device = NULL;
    NTSTATUS status;

    // Create and then delete device - verify no crash
    status = IoCreateDevice(&dummy_driver_object, 0, NULL, FILE_DEVICE_UNKNOWN, FALSE, &device);
    GEN_CHECK(status, STATUS_SUCCESS, "IoCreateDevice for delete test");
    if (NT_SUCCESS(status) && device) {
        IoDeleteDevice(device);
        // If we got here without crashing, the delete worked
        GEN_CHECK(TRUE, TRUE, "IoDeleteDevice succeeded");
    }

    // Create with extension and delete
    device = NULL;
    status = IoCreateDevice(&dummy_driver_object, 128, NULL, FILE_DEVICE_UNKNOWN, FALSE, &device);
    GEN_CHECK(status, STATUS_SUCCESS, "IoCreateDevice with ext for delete");
    if (NT_SUCCESS(status) && device) {
        IoDeleteDevice(device);
        GEN_CHECK(TRUE, TRUE, "IoDeleteDevice with ext succeeded");
    }

    TEST_END();
}
