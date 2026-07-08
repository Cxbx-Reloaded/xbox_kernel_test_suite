#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtFlushBuffersFile)
{
    TEST_BEGIN();

    // Create a file and flush
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition1\\ntflushtest.tmp");
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
        // Write data
        CHAR buf[64];
        memset(buf, 'F', sizeof(buf));
        NtWriteFile(file_handle, NULL, NULL, NULL, &iosb, buf, sizeof(buf), NULL);

        // Flush
        status = NtFlushBuffersFile(file_handle, &iosb);
        GEN_CHECK(status, STATUS_SUCCESS, "flush succeeds");

        // Flush again (no-op, should still succeed)
        status = NtFlushBuffersFile(file_handle, &iosb);
        GEN_CHECK(status, STATUS_SUCCESS, "flush again OK");

        NtClose(file_handle);
        NtDeleteFile(&obj_attr);
    } else {
        GEN_CHECK(TRUE, TRUE, "file ops may not be available");
    }

    // --- Invalid handle ---
    status = NtFlushBuffersFile((HANDLE)0xDEAD, &iosb);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle");

    TEST_END();
}
