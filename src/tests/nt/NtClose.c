#include <hal/fileio.h>

#include "util/output.h"
#include "assertions/common.h"

TEST_FUNC(NtClose)
{
    TEST_BEGIN();

    // TODO: Update this test with more macros checks.
    HANDLE handle;
    NTSTATUS status;
    OBJECT_ATTRIBUTES obj;
    IO_STATUS_BLOCK isb;
    ANSI_STRING obj_name;
    char filepath[200];

    XConvertDOSFilenameToXBOX("./default.xbe", filepath);
    RtlInitAnsiString(&obj_name, filepath);

    obj.RootDirectory = NULL;
    obj.Attributes = OBJ_CASE_INSENSITIVE;
    obj.ObjectName = &obj_name;

    status = NtCreateFile(&handle,
                          GENERIC_READ,
                          &obj,
                          &isb,
                          NULL,
                          FILE_ATTRIBUTE_NORMAL,
                          0,
                          FILE_OPEN,
                          FILE_SYNCHRONOUS_IO_NONALERT);

    status = NtClose(handle);
    test_passed &= assert_NTSTATUS(status, STATUS_SUCCESS, "NtClose");

    TEST_END();
}
