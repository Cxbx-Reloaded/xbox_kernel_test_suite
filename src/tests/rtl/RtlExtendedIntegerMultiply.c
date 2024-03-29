#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(RtlExtendedIntegerMultiply)
{
    TEST_BEGIN();

    typedef struct _int_multiply_test {
        const LARGE_INTEGER multiplicand;
        const LONG multiplier;
        const LONGLONG expected_result;
        LARGE_INTEGER return_result;
    } int_multiply_test;

    int_multiply_test int_multiply_tests[] = {
        { .multiplicand.QuadPart = 1, .multiplier = 0, .expected_result = 0 },
        { .multiplicand.QuadPart = 0, .multiplier = -1, .expected_result = 0 },
        { .multiplicand.QuadPart = 15, .multiplier = 15, .expected_result = 225 },
        { .multiplicand.QuadPart = 15, .multiplier = -15, .expected_result = -225 },
        { .multiplicand.QuadPart = -15, .multiplier = -15, .expected_result = 225 }
    };
    unsigned int_multiply_tests_size = ARRAY_SIZE(int_multiply_tests);

    for (unsigned i = 0; i < ARRAY_SIZE(int_multiply_tests); i++) {
        int_multiply_tests[i].return_result = RtlExtendedIntegerMultiply(int_multiply_tests[i].multiplicand, int_multiply_tests[i].multiplier);
    }
    GEN_CHECK_ARRAY_MEMBER(int_multiply_tests, return_result.QuadPart, expected_result, int_multiply_tests_size, "int_multiply_tests");

    TEST_END();
}
