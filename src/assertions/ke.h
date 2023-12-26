#pragma once

#include <xboxkrnl/xboxkrnl.h>

BOOL assert_critical_region_ex(PKTHREAD, ULONG, const char*, int);
#define assert_critical_region( \
    thread, \
    expected_Apc, \
    test_name \
) \
assert_critical_region_ex( \
    thread, \
    expected_Apc, \
    test_name, \
    __LINE__ \
)
