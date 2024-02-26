#pragma once

#include <xboxkrnl/xboxkrnl.h>

BOOL assert_critical_section_equals_ex(
    PRTL_CRITICAL_SECTION,
    LONG,
    LONG,
    HANDLE,
    const char*,
    int
);
#define assert_critical_section_equals( \
    crit_section, \
    expected_LockCount, \
    expected_RecursionCount, \
    expected_OwningThread, \
    test_name \
) \
test_passed &= assert_critical_section_equals_ex( \
    crit_section, \
    expected_LockCount, \
    expected_RecursionCount, \
    expected_OwningThread, \
    test_name, \
    __LINE__ \
)

BOOL assert_ansi_string_ex(
    PANSI_STRING,
    USHORT,
    USHORT,
    PCHAR,
    const char*,
    int
);
#define assert_ansi_string( \
    string, \
    expected_Length, \
    expected_MaximumLength, \
    expected_Buffer, \
    test_name \
) \
test_passed &= assert_ansi_string_ex( \
    string, \
    expected_Length, \
    expected_MaximumLength, \
    expected_Buffer, \
    test_name, \
    __LINE__ \
)

BOOL assert_unicode_string_ex(
    PUNICODE_STRING,
    USHORT,
    USHORT,
    PWSTR,
    const char*,
    int
);
#define assert_unicode_string( \
    string, \
    expected_Length, \
    expected_MaximumLength, \
    expected_Buffer, \
    test_name \
) \
test_passed &= assert_unicode_string_ex( \
    string, \
    expected_Length, \
    expected_MaximumLength, \
    expected_Buffer, \
    test_name, \
    __LINE__ \
)
