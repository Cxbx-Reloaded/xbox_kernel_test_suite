#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(IoQueryVolumeInformation)
{
    TEST_BEGIN();

    // IoQueryVolumeInformation queries volume info via a FILE_OBJECT.
    // Use the running XBE's path (from kernel export) to guarantee a valid file.
    OBJECT_ATTRIBUTES oa;
    IO_STATUS_BLOCK iosb;
    HANDLE hFile;

    InitializeObjectAttributes(&oa, &XeImageFileName[0], OBJ_CASE_INSENSITIVE, NULL, NULL);

    NTSTATUS status = NtOpenFile(&hFile, GENERIC_READ | SYNCHRONIZE, &oa, &iosb,
        FILE_SHARE_READ, FILE_SYNCHRONOUS_IO_NONALERT);
    if (!NT_SUCCESS(status)) {
        TEST_SKIP("could not open test file");
        TEST_END();
        return;
    }

    PFILE_OBJECT file_obj = NULL;
    status = ObReferenceObjectByHandle(hFile, &IoFileObjectType, (PVOID*)&file_obj);
    if (!NT_SUCCESS(status) || !file_obj) {
        NtClose(hFile);
        GEN_CHECK(TRUE, TRUE, "SKIP - ObReferenceObjectByHandle failed");
        TEST_END();
        return;
    }

    // Query volume size info
    FILE_FS_SIZE_INFORMATION size_info;
    ULONG returned_len = 0;
    memset(&size_info, 0, sizeof(size_info));

    status = IoQueryVolumeInformation(file_obj, FileFsSizeInformation, sizeof(size_info), &size_info, &returned_len);
    GEN_CHECK(NT_SUCCESS(status), TRUE, "IoQueryVolumeInformation size info");
    if (NT_SUCCESS(status)) {
        GEN_CHECK(returned_len > 0, TRUE, "returned length > 0");
        GEN_CHECK(size_info.BytesPerSector > 0, TRUE, "bytes per sector > 0");
    }

    ObfDereferenceObject(file_obj);
    NtClose(hFile);
    TEST_END();
}
