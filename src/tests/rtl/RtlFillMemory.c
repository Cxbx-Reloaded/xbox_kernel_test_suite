#include <xboxkrnl/xboxkrnl.h>
#include <stdint.h>

#include "global.h" // for (passed|failed)_test vars
#include "util/output.h"

void test_RtlFillMemory()
{
    const char* func_num = "0x011C";
    const char* func_name = "RtlFillMemory";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const BYTE buf_len = 20;
    BYTE buffer[buf_len];
    DWORD lengths[] = {1, 10, buf_len};
    BYTE fills[] = {0x1, 0x2A, 0xFF};
    BOOL individual_test_passes = 1;

    RtlZeroMemory(buffer, buf_len);
    for(uint8_t i = 0; i < sizeof(lengths) / sizeof(DWORD); i++) {
        const char* result_text = passed_text;
        RtlFillMemory(buffer, lengths[i], fills[i]);
        for(uint8_t y = 0; y < buf_len; y++) {
            BYTE expected_fill = (y < lengths[i]) ? fills[i] : 0x00;
            if(buffer[y] != expected_fill) {
                print("  ERROR: For index = %u, got = 0x%x, expected = 0x%x", y, buffer[y], expected_fill);
                individual_test_passes = 0;
            }
        }
        if(!individual_test_passes) {
            tests_passed = 0;
            result_text = failed_text;
        }
        print("  Test %s for length = %u, fill = 0x%x", result_text, lengths[i], fills[i]);
        individual_test_passes = 1;
    }

    print_test_footer(func_num, func_name, tests_passed);
}
