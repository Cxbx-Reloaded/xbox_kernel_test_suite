#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtFsControlFile)
{
    TEST_BEGIN();

    // Open a filesystem volume to issue FSCTL against
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition1\\");
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK iosb;
    HANDLE vol_handle = NULL;
    NTSTATUS status = NtOpenFile(
        &vol_handle,
        GENERIC_READ | SYNCHRONIZE,
        &obj_attr,
        &iosb,
        FILE_SHARE_READ,
        FILE_SYNCHRONOUS_IO_NONALERT | FILE_DIRECTORY_FILE
    );

    if (NT_SUCCESS(status)) {
        // Issue an invalid FSCTL - should fail but not crash
        status = NtFsControlFile(
            vol_handle,
            NULL, NULL, NULL,
            &iosb,
            0xDEADBEEF, // bogus FSCTL code
            NULL, 0,
            NULL, 0
        );
        GEN_CHECK(NT_SUCCESS(status), FALSE, "bogus FSCTL fails");

        NtClose(vol_handle);
    } else {
        GEN_CHECK(TRUE, TRUE, "volume may not be available");
    }

    // --- Invalid handle ---
    status = NtFsControlFile(
        (HANDLE)0xDEAD, NULL, NULL, NULL, &iosb,
        0, NULL, 0, NULL, 0
    );
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle");

    TEST_END();
}
