#include <hal/fileio.h>

#include "util/output.h"

void test_NtCreateFile(){
    const char* func_num = "0x00BE";
    const char* func_name = "NtCreateFile";
    HANDLE handle;
    NTSTATUS status;
    OBJECT_ATTRIBUTES obj;
    IO_STATUS_BLOCK isb;
    ANSI_STRING obj_name;
    char filepath[200];
    BOOL tests_passed = 1;

    print_test_header(func_num, func_name);

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

    tests_passed &= NT_SUCCESS(status);
    NtClose(handle);

    print_test_footer(func_num, func_name, tests_passed);
}
