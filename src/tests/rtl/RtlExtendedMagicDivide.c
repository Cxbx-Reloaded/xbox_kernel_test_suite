#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(RtlExtendedMagicDivide)
{
    TEST_BEGIN();

    typedef struct _magic_div_test {
        LARGE_INTEGER dividend;
        LARGE_INTEGER magic_divisor;
        CCHAR shift_count;
        LONGLONG expected_result;
        LARGE_INTEGER return_result;
    } magic_div_test;

    // RtlExtendedMagicDivide computes: (Dividend * MagicDivisor) >> (64 + ShiftCount)
    // This is used internally for time conversions.
    // Magic number for dividing by 10000000 (100ns to seconds), from NT kernel time.c
    magic_div_test tests[] = {
        // 10000000 / 10000000 = 1 (using magic for div by 10000000)
        { .dividend.QuadPart = 10000000LL, .magic_divisor.QuadPart = (LONGLONG)0xd6bf94d5e57a42bdULL, .shift_count = 23, .expected_result = 1 },
        // 100000000 / 10000000 = 10
        { .dividend.QuadPart = 100000000LL, .magic_divisor.QuadPart = (LONGLONG)0xd6bf94d5e57a42bdULL, .shift_count = 23, .expected_result = 10 },
        // 0 / anything = 0
        { .dividend.QuadPart = 0, .magic_divisor.QuadPart = (LONGLONG)0xd6bf94d5e57a42bdULL, .shift_count = 23, .expected_result = 0 },
        // Simple test: magic for dividing by 10: magic=0xCCCCCCCCCCCCCCCD, shift=3
        { .dividend.QuadPart = 100, .magic_divisor.QuadPart = (LONGLONG)0xCCCCCCCCCCCCCCCDULL, .shift_count = 3, .expected_result = 10 },
        { .dividend.QuadPart = 1000, .magic_divisor.QuadPart = (LONGLONG)0xCCCCCCCCCCCCCCCDULL, .shift_count = 3, .expected_result = 100 },
    };

    for (unsigned i = 0; i < ARRAY_SIZE(tests); i++) {
        tests[i].return_result = RtlExtendedMagicDivide(
            tests[i].dividend,
            tests[i].magic_divisor,
            tests[i].shift_count
        );
    }
    GEN_CHECK_ARRAY_MEMBER(tests, return_result.QuadPart, expected_result, ARRAY_SIZE(tests), "magic_div_tests");

    TEST_END();
}
