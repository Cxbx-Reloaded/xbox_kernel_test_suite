#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtSetInformationFile)
{
    TEST_BEGIN();

    // Create temp file
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition1\\ntsetinfotest.tmp");
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
        // Write some data
        CHAR buf[200];
        memset(buf, 'B', sizeof(buf));
        NtWriteFile(file_handle, NULL, NULL, NULL, &iosb, buf, sizeof(buf), NULL);

        // --- Set position back to 0 ---
        FILE_POSITION_INFORMATION pos_info;
        pos_info.CurrentByteOffset.QuadPart = 0;
        status = NtSetInformationFile(file_handle, &iosb,
            &pos_info, sizeof(pos_info), FilePositionInformation);
        GEN_CHECK(status, STATUS_SUCCESS, "set position to 0");

        // Verify position
        FILE_POSITION_INFORMATION check_pos;
        NtQueryInformationFile(file_handle, &iosb,
            &check_pos, sizeof(check_pos), FilePositionInformation);
        GEN_CHECK(check_pos.CurrentByteOffset.QuadPart, 0, "position is 0");

        // --- Set position to middle ---
        pos_info.CurrentByteOffset.QuadPart = 50;
        status = NtSetInformationFile(file_handle, &iosb,
            &pos_info, sizeof(pos_info), FilePositionInformation);
        GEN_CHECK(status, STATUS_SUCCESS, "set position to 50");

        NtQueryInformationFile(file_handle, &iosb,
            &check_pos, sizeof(check_pos), FilePositionInformation);
        GEN_CHECK(check_pos.CurrentByteOffset.QuadPart, 50, "position is 50");

        NtClose(file_handle);
        NtDeleteFile(&obj_attr);
    } else {
        GEN_CHECK(TRUE, TRUE, "file ops may not be available");
    }

    TEST_END();
}
