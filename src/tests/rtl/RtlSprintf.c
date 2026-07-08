#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(RtlSprintf)
{
    TEST_BEGIN();

    char buf[128];

    // --- %d: signed integers ---
    int ret = RtlSprintf(buf, "%d", 42);
    GEN_CHECK(ret, 2, "%%d 42 return");
    GEN_CHECK(strcmp(buf, "42"), 0, "%%d 42 content");

    ret = RtlSprintf(buf, "%d", -1);
    GEN_CHECK(strcmp(buf, "-1"), 0, "%%d -1");

    ret = RtlSprintf(buf, "%d", 0);
    GEN_CHECK(strcmp(buf, "0"), 0, "%%d 0");

    ret = RtlSprintf(buf, "%d", 2147483647);
    GEN_CHECK(strcmp(buf, "2147483647"), 0, "%%d INT_MAX");

    // --- %u: unsigned ---
    ret = RtlSprintf(buf, "%u", 0xFFFFFFFF);
    GEN_CHECK(strcmp(buf, "4294967295"), 0, "%%u UINT_MAX");

    // --- %x and %X: hex ---
    ret = RtlSprintf(buf, "0x%08X", 0xDEADBEEF);
    GEN_CHECK(ret, 10, "%%X return");
    GEN_CHECK(strcmp(buf, "0xDEADBEEF"), 0, "%%X content");

    ret = RtlSprintf(buf, "%x", 255);
    GEN_CHECK(strcmp(buf, "ff"), 0, "%%x lowercase");

    ret = RtlSprintf(buf, "%x", 0);
    GEN_CHECK(strcmp(buf, "0"), 0, "%%x zero");

    // --- %s: strings ---
    ret = RtlSprintf(buf, "%s", "hello");
    GEN_CHECK(ret, 5, "%%s return");
    GEN_CHECK(strcmp(buf, "hello"), 0, "%%s content");

    ret = RtlSprintf(buf, "%s", "");
    GEN_CHECK(ret, 0, "%%s empty string return");
    GEN_CHECK(buf[0], '\0', "%%s empty null terminated");

    // --- %c: character ---
    ret = RtlSprintf(buf, "%c", 'A');
    GEN_CHECK(ret, 1, "%%c return");
    GEN_CHECK(buf[0], 'A', "%%c content");

    // --- Width/padding ---
    ret = RtlSprintf(buf, "%05d", 42);
    GEN_CHECK(strcmp(buf, "00042"), 0, "zero-padded");

    ret = RtlSprintf(buf, "%-10s!", "hi");
    GEN_CHECK(strcmp(buf, "hi        !"), 0, "left-aligned");

    // --- Multiple arguments ---
    ret = RtlSprintf(buf, "%d %s 0x%x", 10, "ab", 255);
    GEN_CHECK(strcmp(buf, "10 ab 0xff"), 0, "multi args");

    // --- %% literal percent ---
    ret = RtlSprintf(buf, "100%%");
    GEN_CHECK(strcmp(buf, "100%"), 0, "literal percent");

    // --- Return value is character count ---
    ret = RtlSprintf(buf, "abc");
    GEN_CHECK(ret, 3, "return = length");

    // --- NULL format ---
    // SKIP: sprintf with NULL format is undefined behavior, crashes.

    TEST_END();
}
