#include <xboxkrnl/xboxkrnl.h>
#include <stdint.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

// A few things to note about this function:
// 1. Passing NULL into the remainder field is legal.
// 2. If the divisor = 0, then a STATUS_INTEGER_DIVIDE_BY_ZERO exception will be raised. There is no good way
//    to test this as it will cause real hardware to hang.
// 3. Real hardware implements this as a binary divison algorithm that emits interesting results when negative
//    inputs are provided. The expected_results below are taken from what real hardware produces as output.
void test_RtlExtendedLargeIntegerDivide(int func_num, const char* func_name)
{
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    typedef struct _int_divide_test {
        const LARGE_INTEGER dividend;
        const ULONG divisor;
        const ULONG expected_remainder;
        const LONGLONG expected_result;
        LARGE_INTEGER return_result;
        ULONG return_remainder;
    } int_divide_test;

    int_divide_test int_divide_tests[] = {
        { .dividend.QuadPart = 1, .divisor = 1, .expected_remainder = 0, .expected_result = 1 },
        { .dividend.QuadPart = 5, .divisor = 8, .expected_remainder = 5, .expected_result = 0 },
        { .dividend.QuadPart = 300, .divisor = 8, .expected_remainder = 4, .expected_result = 37 },
        { .dividend.QuadPart = -300, .divisor = 8, .expected_remainder = 4, .expected_result = 0x1fffffffffffffda },
        { .dividend.QuadPart = 300, .divisor = -8, .expected_remainder = 300, .expected_result = 0 },
        { .dividend.QuadPart = -300, .divisor = -8, .expected_remainder = -244, .expected_result = 0x100000007 }
    };
    unsigned int_divide_tests_size = ARRAY_SIZE(int_divide_tests);

    for (unsigned i = 0; i < int_divide_tests_size; i++) {
        int_divide_tests[i].return_result = RtlExtendedLargeIntegerDivide(int_divide_tests[i].dividend, int_divide_tests[i].divisor, &int_divide_tests[i].return_remainder);
    }
    GEN_CHECK_ARRAY_MEMBER(int_divide_tests, return_result.QuadPart, expected_result, int_divide_tests_size, "int_divide_tests");
    GEN_CHECK_ARRAY_MEMBER(int_divide_tests, return_remainder, expected_remainder, int_divide_tests_size, "int_divide_tests");

    // Test remainder = NULL
    LARGE_INTEGER dividend = { .QuadPart = 5 };
    LARGE_INTEGER result = RtlExtendedLargeIntegerDivide(dividend, 2, NULL);
    GEN_CHECK(result.QuadPart, 2, "result");

    print_test_footer(func_num, func_name, test_passed);
}
