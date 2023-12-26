#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

void test_RtlUlongByteSwap()
{
    const char* func_num = "0x0133";
    const char* func_name = "RtlUlongByteSwap";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    typedef struct _byte_swap_test {
        const ULONG input;
        const ULONG expected_result;
        ULONG return_result;
    } byte_swap_test;

    byte_swap_test byte_swap_tests[] = {
        { .input = 0xFF00FF00, .expected_result = 0x00FF00FF },
        { .input = 0x12345678, .expected_result = 0x78563412 },
        { .input = 0xD4C3B2A1, .expected_result = 0xA1B2C3D4 }
    };
    unsigned byte_swap_tests_size = ARRAY_SIZE(byte_swap_tests);

    for (unsigned i = 0; i < byte_swap_tests_size; i++) {
        byte_swap_tests[i].return_result = RtlUlongByteSwap(byte_swap_tests[i].input);
    }
    GEN_CHECK_ARRAY_MEMBER(byte_swap_tests, return_result, expected_result, byte_swap_tests_size, "byte_swap_tests");

    print_test_footer(func_num, func_name, test_passed);
}
