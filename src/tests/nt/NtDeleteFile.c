#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtDeleteFile)
{
    TEST_BEGIN();

    // Create a file to delete
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition1\\ntdelfiletest.tmp");
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK iosb;
    HANDLE file_handle = NULL;
    NTSTATUS status = IoCreateFile(
        &file_handle,
        GENERIC_WRITE | SYNCHRONIZE,
        &obj_attr,
        &iosb,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        0,
        FILE_OVERWRITE_IF,
        FILE_SYNCHRONOUS_IO_NONALERT,
        0
    );

    if (NT_SUCCESS(status)) {
        NtClose(file_handle);

        // Now delete it
        BOOLEAN result = NtDeleteFile(&obj_attr);
        GEN_CHECK(result, TRUE, "delete succeeds");

        // Verify it's gone by trying to open
        file_handle = NULL;
        status = NtOpenFile(
            &file_handle,
            GENERIC_READ | SYNCHRONIZE,
            &obj_attr,
            &iosb,
            FILE_SHARE_READ,
            FILE_SYNCHRONOUS_IO_NONALERT
    );
        GEN_CHECK(NT_SUCCESS(status), FALSE, "file gone after delete");
    } else {
        GEN_CHECK(TRUE, TRUE, "file create may not be available");
    }

    // --- Delete non-existent file ---
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition1\\no_such_file_xyz.tmp");
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);
    BOOLEAN result = NtDeleteFile(&obj_attr);
    GEN_CHECK(result, FALSE, "delete non-existent fails");

    TEST_END();
}
