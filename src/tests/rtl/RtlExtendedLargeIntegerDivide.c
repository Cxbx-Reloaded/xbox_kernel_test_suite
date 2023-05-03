#include <xboxkrnl/xboxkrnl.h>
#include <stdint.h>

#include "global.h" // for (passed|failed)_test vars
#include "util/output.h"

// A few things to note about this function:
// 1. Passing NULL into the remainder field is legal.
// 2. If the divisor = 0, then a STATUS_INTEGER_DIVIDE_BY_ZERO exception will be raised. There is no good way
//    to test this as it will cause real hardware to hang.
// 3. Real hardware implements this as a binary divison algorithm that emits interesting results when negative
//    inputs are provided. The expected_results below are taken from what real hardware produces as output.
void test_RtlExtendedLargeIntegerDivide()
{
    const char* func_num = "0x011A";
    const char* func_name = "RtlExtendedLargeIntegerDivide";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const LONGLONG dividends[]        = {1, 5, 300,                -300, 300,        -300};
    const ULONG divisors[]            = {1, 8,   8,                   8,  -8,          -8};
    const ULONG expected_remainders[] = {0, 5,   4,                 0x4, 300,        -244};
    const LONGLONG expected_results[] = {1, 0,  37,  0x1fffffffffffffda,   0, 0x100000007};
    LARGE_INTEGER dividend, result, e_result;
    ULONG remainder;

    for(uint8_t i = 0; i < sizeof(expected_results) / sizeof(LONGLONG); i++) {
        const char* result_text = passed_text;
        dividend.QuadPart = dividends[i];
        e_result.QuadPart = expected_results[i];
        result = RtlExtendedLargeIntegerDivide(dividend, divisors[i], &remainder);
        if((result.QuadPart != e_result.QuadPart) || (remainder != expected_remainders[i])) {
            tests_passed = 0;
            result_text = failed_text;
        }
        // Dec 19th, 2018 - nxdk's printf does not support any format that will print 64 bits.
        // Split the print into the high dword and low dword until nxdk can print them combined.
        print("  Test %s: Expected Result = 0x%x %x with remainder = 0x%x for dividend = 0x%x %x and divisor = 0x%x, result = 0x%x %x with remainder = 0x%x",
              result_text, e_result.u.HighPart, e_result.u.LowPart, expected_remainders[i],
              dividend.u.HighPart, dividend.u.LowPart, divisors[i], result.u.HighPart, result.u.LowPart, remainder);
    }

    // Test remainder = NULL
    dividend.QuadPart = 5;
    result = RtlExtendedLargeIntegerDivide(dividend, 2, NULL);
    if(result.QuadPart != 2) {
        tests_passed = 0;
        print("  Test with NULL remainder FAILED. Expected result = 0x2, got = 0x%x %x",
              result.u.HighPart, result.u.LowPart);
    }
    print_test_footer(func_num, func_name, tests_passed);
}
