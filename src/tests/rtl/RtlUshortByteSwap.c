#include <xboxkrnl/xboxkrnl.h>
#include <stdint.h>

#include "global.h" // for (passed|failed)_test vars
#include "util/output.h"
#include "util/misc.h"

void test_RtlUshortByteSwap()
{
    const char* func_num = "0x013E";
    const char* func_name = "RtlUshortByteSwap";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const USHORT inputs[]           = {0xFF00, 0x00FF, 0x1234, 0xF0E0};
    const USHORT expected_results[] = {0x00FF, 0xFF00, 0x3412, 0xE0F0};
    USHORT result;

    for(uint8_t i = 0; i < ARRAY_SIZE(inputs); i++) {
        const char* result_text = passed_text;
        result = RtlUshortByteSwap(inputs[i]);
        if(result != expected_results[i]) {
            tests_passed = 0;
            result_text = failed_text;
        }
        print("  Test %s: Expected = 0x%x for Input = 0x%x, Result = 0x%x",
              result_text, expected_results[i], inputs[i], result);
    }

    print_test_footer(func_num, func_name, tests_passed);
}
