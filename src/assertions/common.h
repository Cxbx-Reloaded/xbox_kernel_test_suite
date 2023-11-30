#pragma once

#include <xboxkrnl/xboxkrnl.h>

BOOL assert_NTSTATUS_ex(
    NTSTATUS,
    NTSTATUS,
    const char*,
    int
);
#define assert_NTSTATUS( \
    status, \
    expected_status, \
    func_name \
) \
assert_NTSTATUS_ex( \
    status, \
    expected_status, \
    func_name, \
    __LINE__ \
)
