#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(RtlSnprintf)
{
    TEST_BEGIN();

    char buf[32];

    // --- Normal case: fits in buffer ---
    memset(buf, 'X', sizeof(buf));
    int ret = RtlSnprintf(buf, sizeof(buf), "%d", 42);
    GEN_CHECK(ret, 2, "fits return=2");
    GEN_CHECK(strcmp(buf, "42"), 0, "fits content");

    // --- Exact fit (fills buffer completely) ---
    memset(buf, 'X', sizeof(buf));
    ret = RtlSnprintf(buf, 6, "hello");
    GEN_CHECK(ret, 5, "exact fit return");
    GEN_CHECK(strcmp(buf, "hello"), 0, "exact fit content");

    // --- Truncation: returns -1 (MSVC _snprintf behavior) ---
    memset(buf, 'X', sizeof(buf));
    ret = RtlSnprintf(buf, 4, "%s", "hello world");
    GEN_CHECK(ret, -1, "truncated return");
    GEN_CHECK(buf[0], 'h', "trunc char 0");
    GEN_CHECK(buf[1], 'e', "trunc char 1");
    GEN_CHECK(buf[2], 'l', "trunc char 2");
    GEN_CHECK(buf[3], 'l', "trunc char 3");

    // --- Size 1: can fit only null terminator or one char ---
    memset(buf, 'X', sizeof(buf));
    ret = RtlSnprintf(buf, 1, "AB");
    GEN_CHECK(ret, -1, "size 1 truncated");
    GEN_CHECK(buf[0], 'A', "size 1 first char");

    // --- Size 0: nothing written ---
    memset(buf, 'X', sizeof(buf));
    ret = RtlSnprintf(buf, 0, "%d", 99);
    GEN_CHECK(ret, -1, "size 0 return");
    GEN_CHECK(buf[0], 'X', "size 0 no write");

    // --- Empty format string ---
    memset(buf, 'X', sizeof(buf));
    ret = RtlSnprintf(buf, sizeof(buf), "");
    GEN_CHECK(ret, 0, "empty format return=0");
    GEN_CHECK(buf[0], '\0', "empty format null term");

    // --- Large format, small buffer ---
    memset(buf, 'X', sizeof(buf));
    ret = RtlSnprintf(buf, 8, "%08X", 0xDEADBEEF);
    GEN_CHECK(ret, 8, "8-char hex in 8-byte buf");

    // --- Multiple arguments ---
    memset(buf, 'X', sizeof(buf));
    ret = RtlSnprintf(buf, sizeof(buf), "%d %s", 10, "ok");
    GEN_CHECK(strcmp(buf, "10 ok"), 0, "multi args");
    GEN_CHECK(ret, 5, "multi args return");

    // --- Buffer doesn't overflow past size ---
    memset(buf, 'Z', sizeof(buf));
    RtlSnprintf(buf, 3, "ABCDEF");
    GEN_CHECK(buf[3], 'Z', "no overflow past size");

    TEST_END();
}
