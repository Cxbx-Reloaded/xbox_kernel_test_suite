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
        IoInvalidDeviceRequest, // IRP_MJ_CREATE
        IoInvalidDeviceRequest, // IRP_MJ_CLOSE
        IoInvalidDeviceRequest, // IRP_MJ_READ
        IoInvalidDeviceRequest, // IRP_MJ_WRITE
        IoInvalidDeviceRequest, // IRP_MJ_QUERY_INFORMATION
        IoInvalidDeviceRequest, // IRP_MJ_SET_INFORMATION
        IoInvalidDeviceRequest, // IRP_MJ_FLUSH_BUFFERS
        IoInvalidDeviceRequest, // IRP_MJ_QUERY_VOLUME_INFORMATION
        IoInvalidDeviceRequest, // IRP_MJ_DIRECTORY_CONTROL
        IoInvalidDeviceRequest, // IRP_MJ_FILE_SYSTEM_CONTROL
        IoInvalidDeviceRequest, // IRP_MJ_DEVICE_CONTROL
        IoInvalidDeviceRequest, // IRP_MJ_INTERNAL_DEVICE_CONTROL
        IoInvalidDeviceRequest, // IRP_MJ_SHUTDOWN
        IoInvalidDeviceRequest  // IRP_MJ_CLEANUP
    }
};
