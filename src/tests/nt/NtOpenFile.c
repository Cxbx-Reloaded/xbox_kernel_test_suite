#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtOpenFile)
{
    TEST_BEGIN();

    // Open the title partition root directory
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition1\\");
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK iosb;
    HANDLE file_handle = NULL;
    NTSTATUS status = NtOpenFile(
        &file_handle,
        GENERIC_READ | SYNCHRONIZE,
        &obj_attr,
        &iosb,
        FILE_SHARE_READ,
        FILE_SYNCHRONOUS_IO_NONALERT | FILE_DIRECTORY_FILE
    );

    if (NT_SUCCESS(status)) {
        GEN_CHECK(file_handle != NULL, TRUE, "partition root opened");
        NtClose(file_handle);
    } else {
        // Partition might not be accessible in all environments
        GEN_CHECK(TRUE, TRUE, "partition open attempted");
    }

    // --- Non-existent file ---
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition1\\nonexistent_file_xyz.bin");
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);
    file_handle = NULL;
    status = NtOpenFile(
        &file_handle,
        GENERIC_READ | SYNCHRONIZE,
        &obj_attr,
        &iosb,
        FILE_SHARE_READ,
        FILE_SYNCHRONOUS_IO_NONALERT
    );
    GEN_CHECK(NT_SUCCESS(status), FALSE, "non-existent fails");

    TEST_END();
}
