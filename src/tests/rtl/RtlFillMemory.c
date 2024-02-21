#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(RtlFillMemory)
{
    TEST_BEGIN();

    enum { buf_len = 20 };
    UCHAR buffer[buf_len] = { 0 };
    UCHAR expected_fills[buf_len] = { 0 };

    typedef struct _fill_test {
        DWORD length;
        UCHAR fill;
    } fill_test;

    fill_test fill_tests[] = {
        { .length = 1, .fill = 0x1 },
        { .length = 10, .fill = 0x2A },
        { .length = buf_len, .fill = 0xFF }
    };

    RtlZeroMemory(buffer, buf_len);
    for (unsigned i = 0; i < ARRAY_SIZE(fill_tests); i++) {
        // setup expected fill array
        for (unsigned k = 0; k < fill_tests[i].length; k++) {
            expected_fills[k] = fill_tests[i].fill;
        }
        RtlFillMemory(buffer, fill_tests[i].length * sizeof(UCHAR), fill_tests[i].fill);
        GEN_CHECK_ARRAY(buffer, expected_fills, buf_len, "fills");
    }

    TEST_END();
}
