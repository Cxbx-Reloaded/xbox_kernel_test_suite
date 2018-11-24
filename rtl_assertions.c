#include "rtl_assertions.h"
#include "assertion_defines.h"

#include <string.h>

BOOL assert_critical_section_equals(
    PRTL_CRITICAL_SECTION crit_section,
    LONG expected_LockCount,
    LONG expected_RecursionCount,
    HANDLE expected_OwningThread,
    const char* test_name
) {
    ASSERT_HEADER

    GEN_CHECK(crit_section->LockCount, expected_LockCount, "LockCount")
    GEN_CHECK(crit_section->RecursionCount, expected_RecursionCount, "RecursionCount")
    GEN_CHECK(crit_section->OwningThread, expected_OwningThread, "OwningThread")

    ASSERT_FOOTER(test_name)
}

BOOL assert_ansi_string(
    PANSI_STRING string,
    USHORT expected_Length,
    USHORT expected_MaximumLength,
    PCHAR expected_Buffer,
    const char* test_name
) {
    ASSERT_HEADER

    GEN_CHECK(string->Length, expected_Length, "Length");
    GEN_CHECK(string->MaximumLength, expected_MaximumLength, "MaximumLength");

    if(expected_Buffer == NULL) {
        GEN_CHECK(string->Buffer, NULL, "Buffer is NULL");
    }
    else {
        int result = strncmp(string->Buffer, expected_Buffer, expected_MaximumLength);
        GEN_CHECK(result, 0, "strcmp result of Buffer");
        if(result) {
            print("  Buffer = %s, expected_Buffer = %s", string->Buffer, expected_Buffer);
        }
    }

    ASSERT_FOOTER(test_name)
}

static BOOL assert_unicode_string(
    PUNICODE_STRING string,
    USHORT expected_Length,
    USHORT expected_MaximumLength,
    PWSTR expected_Buffer,
    const char* test_name
) {
    ASSERT_HEADER

    GEN_CHECK(string->Length, expected_Length, "Length");
    GEN_CHECK(string->MaximumLength, expected_MaximumLength, "MaximumLength");
    GEN_CHECK(string->Buffer, expected_Buffer, "Buffer");

    ASSERT_FOOTER(test_name)
}

BOOL assert_rtl_compared_bytes(
    SIZE_T num_matching_bytes,
    SIZE_T expected_matching_bytes,
    const char* test_name
) {
    ASSERT_HEADER

    GEN_CHECK(num_matching_bytes, expected_matching_bytes, "num_matching_bytes");

    ASSERT_FOOTER(test_name)
}
