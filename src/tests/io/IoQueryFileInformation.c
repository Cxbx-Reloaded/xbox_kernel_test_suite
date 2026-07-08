#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(IoQueryFileInformation)
{
    TEST_BEGIN();

    // IoQueryFileInformation queries info on a FILE_OBJECT.
    // We need a real file object to test this.
    OBJECT_ATTRIBUTES oa;
    OBJECT_STRING path;
    IO_STATUS_BLOCK iosb;
    HANDLE hFile;

    RtlInitAnsiString(&path, "\\Device\\Harddisk0\\Partition1\\devkit\\default.xbe");
    InitializeObjectAttributes(&oa, &path, OBJ_CASE_INSENSITIVE, NULL, NULL);

    NTSTATUS status = NtOpenFile(&hFile, GENERIC_READ | SYNCHRONIZE, &oa, &iosb,
        FILE_SHARE_READ, FILE_SYNCHRONOUS_IO_NONALERT);
    if (!NT_SUCCESS(status)) {
        GEN_CHECK(TRUE, TRUE, "SKIP - could not open test file");
        TEST_END();
        return;
    }

    // Get file object from handle
    PFILE_OBJECT file_obj = NULL;
    status = ObReferenceObjectByHandle(hFile, &IoFileObjectType, (PVOID*)&file_obj);
    if (!NT_SUCCESS(status) || !file_obj) {
        NtClose(hFile);
        GEN_CHECK(TRUE, TRUE, "SKIP - ObReferenceObjectByHandle failed");
        TEST_END();
        return;
    }

    // Query basic info
    FILE_BASIC_INFORMATION basic_info;
    ULONG returned_len = 0;
    memset(&basic_info, 0, sizeof(basic_info));

    status = IoQueryFileInformation(file_obj, FileBasicInformation, sizeof(basic_info), &basic_info, &returned_len);
    GEN_CHECK(NT_SUCCESS(status), TRUE, "IoQueryFileInformation basic");
    if (NT_SUCCESS(status)) {
        GEN_CHECK(returned_len, sizeof(basic_info), "returned length");
    }

    ObfDereferenceObject(file_obj);
    NtClose(hFile);
    TEST_END();
}
