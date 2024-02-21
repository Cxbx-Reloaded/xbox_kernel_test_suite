#include <hal/fileio.h>

#include "util/output.h"

TEST_FUNC(NtReadFile)
{
    TEST_BEGIN();

    // We will read the xbe magic number as a test
    HANDLE handle;
    NTSTATUS status;
    OBJECT_ATTRIBUTES obj;
    IO_STATUS_BLOCK isb;
    ANSI_STRING obj_name;
    char filepath[200];
    ULONG uSize = 0x0004;
    char read[0x0004];

    XConvertDOSFilenameToXBOX("./default.xbe", filepath);
    RtlInitAnsiString(&obj_name, filepath);

    obj.RootDirectory = NULL;
    obj.Attributes = OBJ_CASE_INSENSITIVE;
    obj.ObjectName = &obj_name;

    status = NtCreateFile(
    &handle,
    GENERIC_READ,
    &obj,
    &isb,
    NULL,
    FILE_ATTRIBUTE_NORMAL,
    0,
    FILE_OPEN,
    FILE_SYNCHRONOUS_IO_NONALERT);

    if (!NT_SUCCESS(status)) {
        NtClose(handle);
        test_passed = 0;
        TEST_END();
        return;
    }

    status = NtReadFile(
    handle,
    NULL,
    NULL,
    NULL,
    &isb,
    read,
    uSize,
    NULL);

    if (status == STATUS_PENDING)
        status = NtWaitForSingleObject((void*)handle, FALSE, (void*)NULL);

    test_passed &= NT_SUCCESS(status);
    NtClose(handle);

    TEST_END();
}
