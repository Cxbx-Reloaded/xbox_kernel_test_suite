#include <xboxkrnl/xboxkrnl.h>
#include <stdint.h>

#include "global.h" // for (passed|failed)_test vars
#include "util/output.h"
#include "util/misc.h"

void test_RtlExtendedIntegerMultiply()
{
    const char* func_num = "0x0119";
    const char* func_name = "RtlExtendedIntegerMultiply";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const LONGLONG multiplicands[]    = {1,  0,  15,   15, -15};
    const LONG multipliers[]          = {0, -1,  15,  -15, -15};
    const LONGLONG expected_results[] = {0,  0, 225, -225, 225};
    LARGE_INTEGER multiplicand, result;

    for(uint8_t i = 0; i < ARRAY_SIZE(expected_results); i++) {
        const char* result_text = passed_text;
        multiplicand.QuadPart = multiplicands[i];
        result = RtlExtendedIntegerMultiply(multiplicand, multipliers[i]);
        if(result.QuadPart != expected_results[i]) {
            tests_passed = 0;
            result_text = failed_text;
        }
        print("  Test %s: Expected = %d for multiplicand = %d and multipler = %d, result = %d",
              result_text, (LONG)expected_results[i], (LONG)multiplicands[i], (LONG)multipliers[i], (LONG)result.QuadPart);
    }
    print_test_footer(func_num, func_name, tests_passed);
}
