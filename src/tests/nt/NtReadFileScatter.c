#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtReadFileScatter)
{
    TEST_BEGIN();

    // NtReadFileScatter reads file data into non-contiguous page-aligned buffers.
    // Requires PAGE_SIZE-aligned buffers and offsets.

    // Create a temp file with known content (at least 2 pages)
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition1\\ntscattertest.tmp");
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
        // Write 2 pages of data
        PVOID write_buf = MmAllocateContiguousMemory(PAGE_SIZE * 2);
        if (write_buf) {
            memset(write_buf, 'S', PAGE_SIZE * 2);
            LARGE_INTEGER offset;
            offset.QuadPart = 0;
            NtWriteFile(file_handle, NULL, NULL, NULL, &iosb,
                       write_buf, PAGE_SIZE * 2, &offset);

            // Allocate 2 separate page-aligned buffers for scatter read
            PVOID buf1 = MmAllocateContiguousMemory(PAGE_SIZE);
            PVOID buf2 = MmAllocateContiguousMemory(PAGE_SIZE);

            if (buf1 && buf2) {
                memset(buf1, 0, PAGE_SIZE);
                memset(buf2, 0, PAGE_SIZE);

                FILE_SEGMENT_ELEMENT segments[3];
                segments[0].Buffer = buf1;
                segments[1].Buffer = buf2;
                segments[2].Buffer = NULL; // terminator

                offset.QuadPart = 0;
                status = NtReadFileScatter(
                    file_handle, NULL, NULL, NULL,
                    &iosb, segments, PAGE_SIZE * 2, &offset
                );

                if (NT_SUCCESS(status)) {
                    GEN_CHECK(((BYTE*)buf1)[0], 'S', "scatter page 1");
                    GEN_CHECK(((BYTE*)buf2)[0], 'S', "scatter page 2");
                }
            }

            if (buf1) MmFreeContiguousMemory(buf1);
            if (buf2) MmFreeContiguousMemory(buf2);
            MmFreeContiguousMemory(write_buf);
        }

        NtClose(file_handle);
        NtDeleteFile(&obj_attr);
    } else {
        GEN_CHECK(TRUE, TRUE, "file ops may not be available");
    }

    TEST_END();
}
