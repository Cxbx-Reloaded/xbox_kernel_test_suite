#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"
#include "util/device_dummy.h"

#ifndef FILE_DEVICE_UNKNOWN
#define FILE_DEVICE_UNKNOWN 0x22
#endif

TEST_FUNC(IoCreateDevice)
{
    TEST_BEGIN();

    PDEVICE_OBJECT device = NULL;
    NTSTATUS status;

    // Create unnamed device with no extension
    status = IoCreateDevice(&dummy_driver_object, 0, NULL, FILE_DEVICE_UNKNOWN, FALSE, &device);
    GEN_CHECK(status, STATUS_SUCCESS, "IoCreateDevice unnamed");
    if (NT_SUCCESS(status) && device) {
        GEN_CHECK(device->DriverObject, &dummy_driver_object, "DriverObject set");
        GEN_CHECK(device->DeviceType, FILE_DEVICE_UNKNOWN, "DeviceType");
        GEN_CHECK(device->StackSize, 1, "StackSize == 1");
        GEN_CHECK(device->DeviceExtension, NULL, "No extension");
        GEN_CHECK(device->ReferenceCount, 0, "ReferenceCount == 0");
        IoDeleteDevice(device);
    }

    // Create device with extension
    device = NULL;
    status = IoCreateDevice(&dummy_driver_object, 64, NULL, FILE_DEVICE_UNKNOWN, FALSE, &device);
    GEN_CHECK(status, STATUS_SUCCESS, "IoCreateDevice with ext");
    if (NT_SUCCESS(status) && device) {
        GEN_CHECK(device->DeviceExtension != NULL, TRUE, "Extension non-NULL");
        IoDeleteDevice(device);
    }

    TEST_END();
}
