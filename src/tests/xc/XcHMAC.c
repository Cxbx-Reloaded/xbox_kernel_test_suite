#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(XcHMAC)
{
    TEST_BEGIN();

    // HMAC-SHA1 test vector from RFC 2202:
    // Key = 0x0b repeated 20 times
    // Data = "Hi There"
    // HMAC = B617318655057264E28BC0B6FB378C8EF146BE00
    UCHAR key[20];
    memset(key, 0x0B, 20);

    UCHAR data[] = "Hi There";
    UCHAR expected[] = {
        0xB6, 0x17, 0x31, 0x86, 0x55, 0x05, 0x72, 0x64, 0xE2, 0x8B,
        0xC0, 0xB6, 0xFB, 0x37, 0x8C, 0x8E, 0xF1, 0x46, 0xBE, 0x00
    };

    UCHAR digest[20];
    memset(digest, 0, sizeof(digest));

    XcHMAC(key, 20, data, 8, NULL, 0, digest);

    BOOL matches = (memcmp(digest, expected, 20) == 0);
    GEN_CHECK(matches, TRUE, "HMAC-SHA1 RFC2202 test 1");

    // Test vector 2: Key = "Jefe", Data = "what do ya want for nothing?"
    // HMAC = effcdf6ae5eb2fa2d27416d5f184df9c259a7c79
    UCHAR key2[] = "Jefe";
    UCHAR data2[] = "what do ya want for nothing?";
    UCHAR expected2[] = {
        0xEF, 0xFC, 0xDF, 0x6A, 0xE5, 0xEB, 0x2F, 0xA2, 0xD2, 0x74,
        0x16, 0xD5, 0xF1, 0x84, 0xDF, 0x9C, 0x25, 0x9A, 0x7C, 0x79
    };

    memset(digest, 0, sizeof(digest));
    XcHMAC(key2, 4, data2, 28, NULL, 0, digest);

    matches = (memcmp(digest, expected2, 20) == 0);
    GEN_CHECK(matches, TRUE, "HMAC-SHA1 RFC2202 test 2");

    // --- Two-part input (pbInput + pbInput2) ---
    // Same as test 2 but split the data
    memset(digest, 0, sizeof(digest));
    XcHMAC(key2, 4, data2, 14, data2 + 14, 14, digest);

    matches = (memcmp(digest, expected2, 20) == 0);
    GEN_CHECK(matches, TRUE, "HMAC-SHA1 split input");

    TEST_END();
}
