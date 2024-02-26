#pragma once

#include <xboxkrnl/xboxkrnl.h>

#include "defines.h"

#define assert_NTSTATUS_ex( \
    status, \
    expected_status, \
    api_name, \
    line_number \
) \
    if(status != expected_status) { \
        print( \
            "  ERROR(line %d): Expected return status of function '%s' = 0x%x, got = 0x%x", \
            line_number, \
            api_name, \
            expected_status, \
            status \
        ); \
        TEST_FAILED(); \
    }
#define assert_NTSTATUS( \
    status, \
    expected_status, \
    api_name \
) \
assert_NTSTATUS_ex( \
    status, \
    expected_status, \
    api_name, \
    __LINE__ \
)
