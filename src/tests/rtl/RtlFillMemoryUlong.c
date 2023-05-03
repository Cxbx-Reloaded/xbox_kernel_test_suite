#include <xboxkrnl/xboxkrnl.h>
#include <stdint.h>

#include "global.h" // for (passed|failed)_test vars
#include "util/output.h"

void test_RtlFillMemoryUlong()
{
    const char* func_num = "0x011D";
    const char* func_name = "RtlFillMemoryUlong";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const BYTE buf_len = 20;
    ULONG buffer[buf_len];
    SIZE_T lengths[] = {1, 10, buf_len};
    ULONG patterns[] = {0x1, 0x2FFF, 0xFFFFFFFF};
    BOOL individual_test_passes = 1;

    RtlZeroMemory(buffer, buf_len * sizeof(ULONG));
    for(uint8_t i = 0; i < sizeof(lengths) / sizeof(SIZE_T); i++) {
        const char* result_text = passed_text;
        RtlFillMemoryUlong(buffer, lengths[i] * sizeof(ULONG), patterns[i]);
        for(uint8_t y = 0; y < buf_len; y++) {
            ULONG expected_pattern = (y < lengths[i]) ? patterns[i] : 0x0;
            if(buffer[y] != expected_pattern) {
                print("  ERROR: For index = %u, got = 0x%x, expected = 0x%x", y, buffer[y], expected_pattern);
                individual_test_passes = 0;
            }
        }
        if(!individual_test_passes) {
            tests_passed = 0;
            result_text = failed_text;
        }
        print("  Test %s for length = %u, pattern = 0x%x", result_text, lengths[i], patterns[i]);
        individual_test_passes = 1;
    }

    print_test_footer(func_num, func_name, tests_passed);
}
