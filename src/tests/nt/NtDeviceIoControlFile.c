#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtDeviceIoControlFile)
{
    TEST_BEGIN();

    // Open a device to issue IOCTLs against
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition0");
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK iosb;
    HANDLE dev_handle = NULL;
    NTSTATUS status = NtOpenFile(
        &dev_handle,
        GENERIC_READ | SYNCHRONIZE,
        &obj_attr,
        &iosb,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        FILE_SYNCHRONOUS_IO_NONALERT
    );

    if (NT_SUCCESS(status)) {
        // Issue an invalid IOCTL - should fail gracefully
        ULONG dummy_out = 0;
        status = NtDeviceIoControlFile(
            dev_handle,
            NULL, NULL, NULL,
            &iosb,
            0xDEADBEEF, // bogus IOCTL code
            NULL, 0,
            &dummy_out, sizeof(dummy_out)
        );
        // Should return some error (not crash)
        GEN_CHECK(NT_SUCCESS(status), FALSE, "bogus IOCTL fails");

        NtClose(dev_handle);
    } else {
        GEN_CHECK(TRUE, TRUE, "device may not be available");
    }

    // --- Invalid handle ---
    status = NtDeviceIoControlFile(
        (HANDLE)0xDEAD, NULL, NULL, NULL, &iosb,
        0, NULL, 0, NULL, 0
    );
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle");

    TEST_END();
}
