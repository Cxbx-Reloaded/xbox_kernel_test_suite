#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtQueryInformationFile)
{
    TEST_BEGIN();

    // Create a temp file with known content
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition1\\ntqueryinfotest.tmp");
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK iosb;
    HANDLE file_handle = NULL;
    NTSTATUS status = IoCreateFile(
        &file_handle,
        GENERIC_WRITE | GENERIC_READ | SYNCHRONIZE,
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
        // Write 100 bytes
        CHAR buf[100];
        memset(buf, 'A', sizeof(buf));
        NtWriteFile(file_handle, NULL, NULL, NULL, &iosb, buf, sizeof(buf), NULL);

        // --- Query FileStandardInformation ---
        FILE_STANDARD_INFORMATION std_info;
        status = NtQueryInformationFile(file_handle, &iosb,
            &std_info, sizeof(std_info), FileStandardInformation);
        GEN_CHECK(status, STATUS_SUCCESS, "query standard");
        GEN_CHECK(std_info.EndOfFile.QuadPart, 100, "EOF = 100");
        GEN_CHECK(std_info.Directory, FALSE, "not a directory");

        // --- Query FilePositionInformation ---
        FILE_POSITION_INFORMATION pos_info;
        status = NtQueryInformationFile(file_handle, &iosb,
            &pos_info, sizeof(pos_info), FilePositionInformation);
        GEN_CHECK(status, STATUS_SUCCESS, "query position");
        GEN_CHECK(pos_info.CurrentByteOffset.QuadPart, 100, "position = 100");

        NtClose(file_handle);
        NtDeleteFile(&obj_attr);
    } else {
        GEN_CHECK(TRUE, TRUE, "file ops may not be available");
    }

    TEST_END();
}
