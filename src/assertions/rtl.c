#include <wchar.h>
#include <string.h>

#include "rtl.h"
#include "defines.h"

BOOL assert_critical_section_equals_ex(
    PRTL_CRITICAL_SECTION crit_section,
    LONG expected_LockCount,
    LONG expected_RecursionCount,
    HANDLE expected_OwningThread,
    const char* test_name,
    int line_number)
{
    ASSERT_HEADER

    GEN_CHECK_EX(crit_section->LockCount, expected_LockCount, "LockCount", line_number);
    GEN_CHECK_EX(crit_section->RecursionCount, expected_RecursionCount, "RecursionCount", line_number);
    GEN_CHECK_EX(crit_section->OwningThread, expected_OwningThread, "OwningThread", line_number);

    ASSERT_FOOTER(test_name)
}

BOOL assert_ansi_string_ex(
    PANSI_STRING string,
    USHORT expected_Length,
    USHORT expected_MaximumLength,
    PCHAR expected_Buffer,
    const char* test_name,
    int line_number)
{
    ASSERT_HEADER

    GEN_CHECK_EX(string->Length, expected_Length, "Length", line_number);
    GEN_CHECK_EX(string->MaximumLength, expected_MaximumLength, "MaximumLength", line_number);

    if(expected_Buffer == NULL) {
        GEN_CHECK_EX(string->Buffer, NULL, "Buffer is NULL", line_number);
    }
    else {
        int result = memcmp(string->Buffer, expected_Buffer, expected_Length);
        GEN_CHECK_EX(result, 0, "memcmp result of Buffer", line_number);
        if(result) {
            print("  Buffer = %s, expected_Buffer = %s", string->Buffer, expected_Buffer);
        }
    }

    ASSERT_FOOTER(test_name)
}

BOOL assert_unicode_string_ex(
    PUNICODE_STRING string,
    USHORT expected_Length,
    USHORT expected_MaximumLength,
    PWSTR expected_Buffer,
    const char* test_name,
    int line_number)
{
    ASSERT_HEADER

    GEN_CHECK_EX(string->Length, expected_Length, "Length", line_number);
    GEN_CHECK_EX(string->MaximumLength, expected_MaximumLength, "MaximumLength", line_number);

    if(expected_Buffer == NULL) {
        GEN_CHECK_EX(string->Buffer, NULL, "Buffer is null", line_number);
    }
    else {
        int result = memcmp(string->Buffer, expected_Buffer, expected_Length);
        GEN_CHECK_EX(result, 0, "memcmp result of Buffer", line_number);
    }

    ASSERT_FOOTER(test_name)
}

BOOL assert_rtl_compared_bytes_ex(
    SIZE_T num_matching_bytes,
    SIZE_T expected_matching_bytes,
    const char* test_name,
    int line_number)
{
    ASSERT_HEADER

    GEN_CHECK_EX(num_matching_bytes, expected_matching_bytes, "num_matching_bytes", line_number);

    ASSERT_FOOTER(test_name)
}
