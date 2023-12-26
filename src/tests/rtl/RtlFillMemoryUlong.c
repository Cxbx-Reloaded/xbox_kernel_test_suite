#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

void test_RtlFillMemoryUlong()
{
    const char* func_num = "0x011D";
    const char* func_name = "RtlFillMemoryUlong";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    enum { buf_len = 20 };
    ULONG buffer[buf_len] = { 0 };
    ULONG expected_fills[buf_len] = { 0 };

    typedef struct _fill_test {
        SIZE_T length;
        ULONG fill;
    } fill_test;

    fill_test fill_tests[] = {
        { .length = 1, .fill = 0x1 },
        { .length = 10, .fill = 0x2FFF },
        { .length = buf_len, .fill = 0xFFFFFFFF }
    };

    RtlZeroMemory(buffer, buf_len * sizeof(ULONG));
    for (unsigned i = 0; i < ARRAY_SIZE(fill_tests); i++) {
        // setup expected fill array
        for (unsigned k = 0; k < fill_tests[i].length; k++) {
            expected_fills[k] = fill_tests[i].fill;
        }
        RtlFillMemoryUlong(buffer, fill_tests[i].length * sizeof(ULONG), fill_tests[i].fill);
        GEN_CHECK_ARRAY(buffer, expected_fills, buf_len, "fills");
    }

    print_test_footer(func_num, func_name, test_passed);
}
