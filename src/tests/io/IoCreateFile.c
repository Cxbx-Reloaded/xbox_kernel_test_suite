#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(IoCreateFile)
{
    TEST_BEGIN();

    HANDLE file_handle = NULL;
    IO_STATUS_BLOCK io_status;
    OBJECT_ATTRIBUTES obj_attr;
    ANSI_STRING file_path;
    NTSTATUS status;

    // Test creating a temporary file
    RtlInitAnsiString(&file_path, "\\Device\\Harddisk0\\Partition1\\xkts_io_test.tmp");
    InitializeObjectAttributes(&obj_attr, &file_path, OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = IoCreateFile(
        &file_handle,
        GENERIC_READ | GENERIC_WRITE,
        &obj_attr,
        &io_status,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        0,
        FILE_OVERWRITE_IF,
        FILE_SYNCHRONOUS_IO_NONALERT,
        0
    );
    GEN_CHECK(status, STATUS_SUCCESS, "IoCreateFile create temp");
    if (NT_SUCCESS(status)) {
        GEN_CHECK(file_handle != NULL, TRUE, "handle non-NULL");
        NtClose(file_handle);
        file_handle = NULL;
    }

    // Re-open existing file
    status = IoCreateFile(
        &file_handle,
        GENERIC_READ,
        &obj_attr,
        &io_status,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ,
        FILE_OPEN,
        FILE_SYNCHRONOUS_IO_NONALERT,
        0
    );
    GEN_CHECK(status, STATUS_SUCCESS, "IoCreateFile re-open");
    if (NT_SUCCESS(status)) {
        NtClose(file_handle);
        file_handle = NULL;
    }

    // Clean up: delete the test file
    InitializeObjectAttributes(&obj_attr, &file_path, OBJ_CASE_INSENSITIVE, NULL, NULL);
    NtDeleteFile(&obj_attr);

    // Test opening non-existent file with FILE_OPEN (should fail)
    RtlInitAnsiString(&file_path, "\\Device\\Harddisk0\\Partition1\\xkts_nonexistent_io.tmp");
    InitializeObjectAttributes(&obj_attr, &file_path, OBJ_CASE_INSENSITIVE, NULL, NULL);
    status = IoCreateFile(
        &file_handle,
        GENERIC_READ,
        &obj_attr,
        &io_status,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        0,
        FILE_OPEN,
        FILE_SYNCHRONOUS_IO_NONALERT,
        0
    );
    GEN_CHECK(status, STATUS_OBJECT_NAME_NOT_FOUND, "IoCreateFile non-existent");

    TEST_END();
}
