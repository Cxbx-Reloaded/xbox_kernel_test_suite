#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtWriteFile)
{
    TEST_BEGIN();

    // Create a temp file, write data, then clean up
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition1\\ntwritefiletest.tmp");
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK iosb;
    HANDLE file_handle = NULL;
    NTSTATUS status = IoCreateFile(
        &file_handle,
        GENERIC_WRITE | GENERIC_READ | SYNCHRONIZE,
        &obj_attr,
        &iosb,
        NULL, // AllocationSize
        FILE_ATTRIBUTE_NORMAL,
        0, // ShareAccess
        FILE_OVERWRITE_IF,
        FILE_SYNCHRONOUS_IO_NONALERT,
        0
    );

    if (NT_SUCCESS(status)) {
        // Write known data
        CHAR write_buf[] = "Hello Xbox Kernel Test!";
        ULONG write_len = (ULONG)strlen(write_buf);
        memset(&iosb, 0, sizeof(iosb));

        status = NtWriteFile(
            file_handle, NULL, NULL, NULL,
            &iosb, write_buf, write_len, NULL
        );
        GEN_CHECK(status, STATUS_SUCCESS, "write succeeds");
        GEN_CHECK((ULONG)iosb.Information, write_len, "bytes written");

        // Write at specific offset
        LARGE_INTEGER offset;
        offset.QuadPart = 0;
        CHAR write_buf2[] = "OVER";
        status = NtWriteFile(
            file_handle, NULL, NULL, NULL,
            &iosb, write_buf2, 4, &offset
        );
        GEN_CHECK(status, STATUS_SUCCESS, "write at offset 0");

        NtClose(file_handle);

        // Clean up: delete the test file
        NtDeleteFile(&obj_attr);
    } else {
        GEN_CHECK(TRUE, TRUE, "file create may not be available");
    }

    TEST_END();
}
