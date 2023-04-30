#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/common.h"

void test_IoDeleteSymbolicLink(){
    const char* func_num = "0x0045";
    const char* func_name = "IoDeleteSymbolicLink";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    ANSI_STRING symlink_str, device_str, null_str, blank_str;
    RtlInitAnsiString(&symlink_str, "\\xkts");
    RtlInitAnsiString(&device_str, "\\Device\\Harddisk0\\Partition5");
    RtlInitAnsiString(&null_str, NULL);
    RtlInitAnsiString(&blank_str, "");
    NTSTATUS result;

    IoCreateSymbolicLink(&symlink_str, &device_str);

    result = IoDeleteSymbolicLink(&symlink_str);
    tests_passed &= assert_NTSTATUS(result, STATUS_SUCCESS, func_name);

    result = IoDeleteSymbolicLink(&symlink_str);
    tests_passed &= assert_NTSTATUS(result, STATUS_OBJECT_NAME_NOT_FOUND, func_name);

    result = IoDeleteSymbolicLink(&null_str);
    tests_passed &= assert_NTSTATUS(result, STATUS_OBJECT_NAME_INVALID, func_name);

    result = IoDeleteSymbolicLink(&blank_str);
    tests_passed &= assert_NTSTATUS(result, STATUS_OBJECT_NAME_INVALID, func_name);

    result = IoDeleteSymbolicLink(NULL);
    tests_passed &= assert_NTSTATUS(result, STATUS_OBJECT_NAME_INVALID, func_name);

    print_test_footer(func_num, func_name, tests_passed);
}
