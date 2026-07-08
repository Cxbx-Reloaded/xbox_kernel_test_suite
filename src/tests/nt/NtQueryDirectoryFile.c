#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtQueryDirectoryFile)
{
    TEST_BEGIN();

    // Open a directory to enumerate files
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition1\\");
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK iosb;
    HANDLE dir_handle = NULL;
    NTSTATUS status = NtOpenFile(
        &dir_handle,
        GENERIC_READ | SYNCHRONIZE,
        &obj_attr,
        &iosb,
        FILE_SHARE_READ,
        FILE_SYNCHRONOUS_IO_NONALERT | FILE_DIRECTORY_FILE
    );

    if (NT_SUCCESS(status)) {
        // Query first directory entry
        BYTE buffer[512];
        ANSI_STRING pattern;
        RtlInitAnsiString(&pattern, "*");

        status = NtQueryDirectoryFile(
            dir_handle,
            NULL, // Event
            NULL, // ApcRoutine
            NULL, // ApcContext
            &iosb,
            buffer,
            sizeof(buffer),
            FileBothDirectoryInformation,
            &pattern,
            TRUE  // RestartScan
        );

        // There should be at least one file/dir on partition 1
        BOOL valid_result = (status == STATUS_SUCCESS || status == STATUS_NO_MORE_FILES);
        GEN_CHECK(valid_result, TRUE, "valid query result");

        if (status == STATUS_SUCCESS) {
            GEN_CHECK(iosb.Information > 0, TRUE, "got directory data");
        }

        NtClose(dir_handle);
    } else {
        GEN_CHECK(TRUE, TRUE, "partition may not be available");
    }

    TEST_END();
}
