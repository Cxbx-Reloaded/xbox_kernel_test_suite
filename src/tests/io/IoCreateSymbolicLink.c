#include <xboxkrnl/xboxkrnl.h>
#include <windows.h>
#include <stddef.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/common.h"

/* possible missing
- STATUS_INSUFFICIENT_RESOURCES
- STATUS_OBJECT_NAME_EXISTS
- STATUS_OBJECT_TYPE_MISMATCH
*/
void test_IoCreateSymbolicLink(int func_num, const char* func_name)
{
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    ANSI_STRING null_str, blank_str, symlink_str, device_str,
        symlink_no_slash_str, symlink_bad_str, device_w_slash_str,
        device_no_slash_str, device_bad_path_nonexistent_str,
        device_bad_path_invalid_str;
    RtlInitAnsiString(&null_str, NULL);
    RtlInitAnsiString(&blank_str, "");
    RtlInitAnsiString(&symlink_str, "\\xkts");
    RtlInitAnsiString(&symlink_no_slash_str, "xkts");
    RtlInitAnsiString(&symlink_bad_str, "\\xkts\\bad_symlink");
    RtlInitAnsiString(&device_str, "\\Device\\Harddisk0\\Partition5");
    RtlInitAnsiString(&device_w_slash_str, "\\Device\\Harddisk0\\Partition5\\");
    RtlInitAnsiString(&device_no_slash_str, "Device\\Harddisk0\\Partition5");
    RtlInitAnsiString(&device_bad_path_nonexistent_str, "\\Device\\Harddisk0\\Partition5\\xkts_nonexistent");
    RtlInitAnsiString(&device_bad_path_invalid_str, "\\Device\\Hardd:sk0\\Partition5");

    NTSTATUS result;

    // First part of the test is ensure we are able to make symbolic link.
    result = IoCreateSymbolicLink(&symlink_str, &device_str);
    tests_passed &= assert_NTSTATUS(result, STATUS_SUCCESS, func_name);

    // Do not delete symbolic link yet in order to trigger object name collision error.
    result = IoCreateSymbolicLink(&symlink_str, &device_str);
    tests_passed &= assert_NTSTATUS(result, STATUS_OBJECT_NAME_COLLISION, func_name);

    // Now we can delete symbolic link for next part of various tests.
    IoDeleteSymbolicLink(&symlink_str);

    // Finally, do various of tests which may could be either successful or failure
    // with cleanup inline to ensure next test is expected to have correct result.
    typedef struct {
        PSTRING pStr;
        NTSTATUS expected_result;
    } str_array;

    str_array symlink_test_str[] = {
        { NULL, STATUS_SUCCESS },
        { &null_str, STATUS_OBJECT_NAME_INVALID },
        { &blank_str, STATUS_OBJECT_NAME_INVALID },
        { &symlink_no_slash_str, STATUS_OBJECT_NAME_INVALID },
        { &symlink_bad_str, STATUS_OBJECT_PATH_NOT_FOUND },
        { &device_str, STATUS_OBJECT_PATH_NOT_FOUND },
        { &device_bad_path_nonexistent_str, STATUS_OBJECT_PATH_NOT_FOUND },
    };
    size_t symlink_test_size = ARRAY_SIZE(symlink_test_str);

    str_array device_test_str[] = {
        { &null_str, STATUS_INVALID_PARAMETER },
        { &blank_str, STATUS_INVALID_PARAMETER },
        { &device_w_slash_str, STATUS_SUCCESS },
        { &device_no_slash_str, STATUS_INVALID_PARAMETER },
        { &device_bad_path_nonexistent_str, STATUS_OBJECT_NAME_NOT_FOUND },
        { &device_bad_path_invalid_str, STATUS_INVALID_PARAMETER },
        { &symlink_str, STATUS_INVALID_PARAMETER },
        { &symlink_no_slash_str, STATUS_INVALID_PARAMETER },
    };
    size_t device_test_str_size = ARRAY_SIZE(device_test_str);

    // Perform tests for symbolic link inputs
    for (unsigned i = 0; i < symlink_test_size; i++) {
        result = IoCreateSymbolicLink(symlink_test_str[i].pStr, &device_str);
        if (result == STATUS_SUCCESS) {
            IoDeleteSymbolicLink(symlink_test_str[i].pStr);
        }

        tests_passed &= assert_NTSTATUS(result, symlink_test_str[i].expected_result, func_name);
    }

    // Perform tests for device inputs
    for (unsigned i = 0; i < device_test_str_size; i++) {
        result = IoCreateSymbolicLink(&symlink_str, device_test_str[i].pStr);
        if (result == STATUS_SUCCESS) {
            IoDeleteSymbolicLink(&symlink_str);
        }

        tests_passed &= assert_NTSTATUS(result, device_test_str[i].expected_result, func_name);
    }

    print_test_footer(func_num, func_name, tests_passed);
}
