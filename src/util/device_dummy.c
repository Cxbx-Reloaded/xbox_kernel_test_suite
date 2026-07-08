#include <xboxkrnl/xboxkrnl.h>

static void NTAPI dummy_start_io(
    IN DEVICE_OBJECT *DeviceObject,
    IN IRP *Irp
)
{
    // Don't need to initialize anything
}

PDEVICE_OBJECT dummy_device_object = NULL;

DRIVER_OBJECT dummy_driver_object = {
    .DriverStartIo = dummy_start_io,
    .DriverDeleteDevice = NULL,
    .DriverDismountVolume = NULL,
    .MajorFunction = {
        IoInvalidDeviceRequest, // 0x00 IRP_MJ_CREATE
        IoInvalidDeviceRequest, // 0x01 IRP_MJ_CLOSE
        IoInvalidDeviceRequest, // 0x02 IRP_MJ_READ
        IoInvalidDeviceRequest, // 0x03 IRP_MJ_WRITE
        IoInvalidDeviceRequest, // 0x04 unknown
        IoInvalidDeviceRequest, // 0x05 unknown
        IoInvalidDeviceRequest, // 0x06 IRP_MJ_FLUSH_BUFFERS
        IoInvalidDeviceRequest, // 0x07 unknown
        IoInvalidDeviceRequest, // 0x08 unknown
        IoInvalidDeviceRequest, // 0x09 unknown
        IoInvalidDeviceRequest, // 0x0A IRP_MJ_DEVICE_CONTROL
        IoInvalidDeviceRequest, // 0x0B IRP_MJ_INTERNAL_DEVICE_CONTROL
        IoInvalidDeviceRequest, // 0x0C unknown
        IoInvalidDeviceRequest  // 0x0D unknown
    }
};
