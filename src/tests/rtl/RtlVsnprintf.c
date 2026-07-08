#include <xboxkrnl/xboxkrnl.h>
#include <string.h>
#include <stdarg.h>

#include "util/output.h"
#include "assertions/defines.h"

static int test_vsnprintf_helper(char* buf, size_t size, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = RtlVsnprintf(buf, size, fmt, args);
    va_end(args);
    return ret;
}

TEST_FUNC(RtlVsnprintf)
{
    TEST_BEGIN();

    char buf[32];

    // Normal case
    int ret = test_vsnprintf_helper(buf, sizeof(buf), "%d %s", 42, "test");
    GEN_CHECK(ret, 7, "vsnprintf return");
    GEN_CHECK(strcmp(buf, "42 test"), 0, "vsnprintf content");

    // Truncation
    ret = test_vsnprintf_helper(buf, 5, "%s", "hello world");
    GEN_CHECK(ret, -1, "vsnprintf truncated");

    TEST_END();
}
