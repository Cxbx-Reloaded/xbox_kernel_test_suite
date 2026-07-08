#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"
#include "assertions/xc.h"

TEST_FUNC(XcSHAInit)
{
    TEST_BEGIN();

    // XcSHAInit initializes a SHA-1 context buffer.
    // Context must be at least 116 bytes.
    UCHAR ctx[116];
    memset(ctx, 0xFF, sizeof(ctx));

    XcSHAInit(ctx);

    // After init, context should be modified (not all 0xFF anymore)
    BOOL modified = FALSE;
    for (ULONG i = 0; i < sizeof(ctx); i++) {
        if (ctx[i] != 0xFF) { modified = TRUE; break; }
    }
    GEN_CHECK(modified, TRUE, "context initialized");

    // The first 5 DWORDs should be the SHA-1 initial hash values
    // H0=0x67452301, H1=0xEFCDAB89, H2=0x98BADCFE, H3=0x10325476, H4=0xC3D2E1F0
    ULONG* state = (ULONG*)ctx;
    GEN_CHECK(state[0], 0x67452301, "H0");
    GEN_CHECK(state[1], 0xEFCDAB89, "H1");
    GEN_CHECK(state[2], 0x98BADCFE, "H2");
    GEN_CHECK(state[3], 0x10325476, "H3");
    GEN_CHECK(state[4], 0xC3D2E1F0, "H4");

    TEST_END();
}

TEST_FUNC(XcSHAUpdate)
{
    TEST_BEGIN();

    // XcSHAUpdate feeds data into the SHA-1 context.
    UCHAR ctx[116];
    XcSHAInit(ctx);

    // Feed some data
    UCHAR data[] = "abc";
    XcSHAUpdate(ctx, data, 3);

    // Context should be different from just-initialized
    UCHAR ctx2[116];
    XcSHAInit(ctx2);
    BOOL different = (memcmp(ctx, ctx2, sizeof(ctx)) != 0);
    GEN_CHECK(different, TRUE, "context changed after update");

    // Multiple updates should accumulate
    UCHAR ctx3[116];
    XcSHAInit(ctx3);
    XcSHAUpdate(ctx3, (PUCHAR)"a", 1);
    XcSHAUpdate(ctx3, (PUCHAR)"b", 1);
    XcSHAUpdate(ctx3, (PUCHAR)"c", 1);

    // Both approaches should produce the same final hash
    UCHAR digest1[20], digest2[20];
    XcSHAFinal(ctx, digest1);
    XcSHAFinal(ctx3, digest2);
    BOOL same = (memcmp(digest1, digest2, 20) == 0);
    GEN_CHECK(same, TRUE, "single vs multi update same result");

    TEST_END();
}

TEST_FUNC(XcSHAFinal)
{
    TEST_BEGIN();

    // Known test vector: SHA-1("abc") = A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
    UCHAR expected_abc[] = {
        0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
        0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D
    };
    assert_hashed_result((PUCHAR)"abc", 3, expected_abc, "SHA1(abc)");

    // SHA-1("") = DA39A3EE 5E6B4B0D 3255BFEF 95601890 AFD80709
    UCHAR expected_empty[] = {
        0xDA, 0x39, 0xA3, 0xEE, 0x5E, 0x6B, 0x4B, 0x0D, 0x32, 0x55,
        0xBF, 0xEF, 0x95, 0x60, 0x18, 0x90, 0xAF, 0xD8, 0x07, 0x09
    };
    assert_hashed_result((PUCHAR)"", 0, expected_empty, "SHA1(empty)");

    // SHA-1("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq")
    // = 84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
    UCHAR long_input[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    UCHAR expected_long[] = {
        0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE,
        0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1
    };
    assert_hashed_result(long_input, (ULONG)strlen((char*)long_input), expected_long, "SHA1(long)");

    TEST_END();
}
