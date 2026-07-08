#include <xboxkrnl/xboxkrnl.h>
#include <string.h>
#include <stdarg.h>

#include "util/output.h"
#include "assertions/defines.h"

static int test_vsprintf_helper(char* buf, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = RtlVsprintf(buf, fmt, args);
    va_end(args);
    return ret;
}

TEST_FUNC(RtlVsprintf)
{
    TEST_BEGIN();

    char buf[64];

    // Basic test
    int ret = test_vsprintf_helper(buf, "%d %s %x", 100, "abc", 0xFF);
    GEN_CHECK(strcmp(buf, "100 abc ff"), 0, "vsprintf content");
    GEN_CHECK(ret, 10, "vsprintf return");

    TEST_END();
}
