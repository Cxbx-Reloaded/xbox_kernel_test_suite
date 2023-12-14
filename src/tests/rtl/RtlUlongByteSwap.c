#include <xboxkrnl/xboxkrnl.h>
#include <stdint.h>

#include "global.h" // for (passed|failed)_test vars
#include "util/output.h"
#include "util/misc.h"

void test_RtlUlongByteSwap()
{
    const char* func_num = "0x0133";
    const char* func_name = "RtlUlongByteSwap";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const ULONG inputs[]           = {0xFF00FF00, 0x12345678, 0xD4C3B2A1};
    const ULONG expected_results[] = {0x00FF00FF, 0x78563412, 0xA1B2C3D4};
    ULONG result;

    for(uint8_t i = 0; i < ARRAY_SIZE(inputs); i++) {
        const char* result_text = passed_text;
        result = RtlUlongByteSwap(inputs[i]);
        if(result != expected_results[i]) {
            tests_passed = 0;
            result_text = failed_text;
        }
        print("  Test %s: Expected = 0x%x for Input = 0x%x, Result = 0x%x",
              result_text, expected_results[i], inputs[i], result);
    }

    print_test_footer(func_num, func_name, tests_passed);
}
