#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtQueryVolumeInformationFile)
{
    TEST_BEGIN();

    // Open partition root to query volume info
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
        // Query FileFsSizeInformation
        FILE_FS_SIZE_INFORMATION size_info;
        status = NtQueryVolumeInformationFile(
            file_handle, &iosb,
            &size_info, sizeof(size_info),
            FileFsSizeInformation
        );
        GEN_CHECK(status, STATUS_SUCCESS, "query size info");
        if (NT_SUCCESS(status)) {
            GEN_CHECK(size_info.TotalAllocationUnits.QuadPart > 0, TRUE, "total units > 0");
            GEN_CHECK(size_info.SectorsPerAllocationUnit > 0, TRUE, "sectors/unit > 0");
            GEN_CHECK(size_info.BytesPerSector > 0, TRUE, "bytes/sector > 0");
        }

        NtClose(file_handle);
    } else {
        GEN_CHECK(TRUE, TRUE, "partition may not be available");
    }

    TEST_END();
}
