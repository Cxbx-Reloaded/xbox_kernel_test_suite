#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

void test_RtlUshortByteSwap()
{
    const char* func_num = "0x013E";
    const char* func_name = "RtlUshortByteSwap";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    typedef struct _byte_swap_test {
        const USHORT input;
        const USHORT expected_result;
        USHORT return_result;
    } byte_swap_test;

    byte_swap_test byte_swap_tests[] = {
        { .input = 0xFF00, .expected_result = 0x00FF },
        { .input = 0x00FF, .expected_result = 0xFF00 },
        { .input = 0x1234, .expected_result = 0x3412 },
        { .input = 0xF0E0, .expected_result = 0xE0F0 }
    };
    unsigned byte_swap_tests_size = ARRAY_SIZE(byte_swap_tests);

    for (unsigned i = 0; i < byte_swap_tests_size; i++) {
        byte_swap_tests[i].return_result = RtlUshortByteSwap(byte_swap_tests[i].input);
    }
    GEN_CHECK_ARRAY_MEMBER(byte_swap_tests, return_result, expected_result, byte_swap_tests_size, "byte_swap_tests");

    print_test_footer(func_num, func_name, test_passed);
}
