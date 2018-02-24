#include "rtl_assertions.h"
#include "assertion_defines.h"

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

static BOOL assert_ansi_string(
    PANSI_STRING string,
    USHORT expected_Length,
    USHORT expected_MaximumLength,
    PCHAR expected_Buffer,
    const char* test_name
) {
    ASSERT_HEADER

    GEN_CHECK(string->Length, expected_Length, "Length");
    GEN_CHECK(string->MaximumLength, expected_MaximumLength, "MaximumLength");
    GEN_CHECK(string->Buffer, expected_Buffer, "Buffer");

    ASSERT_FOOTER(test_name)
}
