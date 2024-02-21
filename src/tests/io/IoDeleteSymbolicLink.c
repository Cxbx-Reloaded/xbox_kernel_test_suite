#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/common.h"

TEST_FUNC(IoDeleteSymbolicLink)
{
    TEST_BEGIN();

    ANSI_STRING symlink_str, device_str, null_str, blank_str;
    RtlInitAnsiString(&symlink_str, "\\xkts");
    RtlInitAnsiString(&device_str, "\\Device\\Harddisk0\\Partition5");
    RtlInitAnsiString(&null_str, NULL);
    RtlInitAnsiString(&blank_str, "");
    NTSTATUS result;

    IoCreateSymbolicLink(&symlink_str, &device_str);

    result = IoDeleteSymbolicLink(&symlink_str);
    test_passed &= assert_NTSTATUS(result, STATUS_SUCCESS, api_name);

    result = IoDeleteSymbolicLink(&symlink_str);
    test_passed &= assert_NTSTATUS(result, STATUS_OBJECT_NAME_NOT_FOUND, api_name);

    result = IoDeleteSymbolicLink(&null_str);
    test_passed &= assert_NTSTATUS(result, STATUS_OBJECT_NAME_INVALID, api_name);

    result = IoDeleteSymbolicLink(&blank_str);
    test_passed &= assert_NTSTATUS(result, STATUS_OBJECT_NAME_INVALID, api_name);

    result = IoDeleteSymbolicLink(NULL);
    test_passed &= assert_NTSTATUS(result, STATUS_OBJECT_NAME_INVALID, api_name);

    TEST_END();
}
