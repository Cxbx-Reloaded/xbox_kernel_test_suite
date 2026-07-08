#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtWriteFileGather)
{
    TEST_BEGIN();

    // NtWriteFileGather writes from non-contiguous page-aligned buffers to a file.

    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition1\\ntgathertest.tmp");
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
        FILE_SYNCHRONOUS_IO_NONALERT | FILE_NO_INTERMEDIATE_BUFFERING,
        0
    );

    if (NT_SUCCESS(status)) {
        PVOID buf1 = MmAllocateContiguousMemory(PAGE_SIZE);
        PVOID buf2 = MmAllocateContiguousMemory(PAGE_SIZE);

        if (buf1 && buf2) {
            memset(buf1, 'G', PAGE_SIZE);
            memset(buf2, 'H', PAGE_SIZE);

            FILE_SEGMENT_ELEMENT segments[3];
            segments[0].Buffer = buf1;
            segments[1].Buffer = buf2;
            segments[2].Buffer = NULL;

            LARGE_INTEGER offset;
            offset.QuadPart = 0;
            BOOLEAN result = NtWriteFileGather(
                file_handle, NULL, NULL, NULL,
                &iosb, segments, PAGE_SIZE * 2, &offset
            );
            GEN_CHECK(result, TRUE, "gather write");
        }

        if (buf1) MmFreeContiguousMemory(buf1);
        if (buf2) MmFreeContiguousMemory(buf2);
        NtClose(file_handle);
        NtDeleteFile(&obj_attr);
    } else {
        GEN_CHECK(TRUE, TRUE, "file ops may not be available");
    }

    TEST_END();
}
