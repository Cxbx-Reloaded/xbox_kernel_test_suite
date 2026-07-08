#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(XcRC4Key)
{
    TEST_BEGIN();

    // XcRC4Key initializes an RC4 key schedule from a key.
    // Key struct is 258 bytes (256-byte S-box + 2 index bytes).
    UCHAR key_struct[258];
    memset(key_struct, 0, sizeof(key_struct));

    UCHAR key[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
    XcRC4Key(key_struct, sizeof(key), key);

    // Key struct should be modified
    BOOL modified = FALSE;
    for (ULONG i = 0; i < sizeof(key_struct); i++) {
        if (key_struct[i] != 0) { modified = TRUE; break; }
    }
    GEN_CHECK(modified, TRUE, "key struct initialized");

    // Same key should produce same key struct
    UCHAR key_struct2[258];
    XcRC4Key(key_struct2, sizeof(key), key);
    BOOL same = (memcmp(key_struct, key_struct2, sizeof(key_struct)) == 0);
    GEN_CHECK(same, TRUE, "deterministic key schedule");

    // Different key should produce different struct
    UCHAR key2[] = { 0x05, 0x04, 0x03, 0x02, 0x01 };
    UCHAR key_struct3[258];
    XcRC4Key(key_struct3, sizeof(key2), key2);
    BOOL different = (memcmp(key_struct, key_struct3, sizeof(key_struct)) != 0);
    GEN_CHECK(different, TRUE, "different key different schedule");

    TEST_END();
}

TEST_FUNC(XcRC4Crypt)
{
    TEST_BEGIN();

    // RC4 known test vector: key="Key", plaintext="Plaintext"
    // Expected ciphertext: BBF316E8 D940AF0A D3
    UCHAR key[] = "Key";
    UCHAR plaintext[] = "Plaintext";
    UCHAR expected_ct[] = { 0xBB, 0xF3, 0x16, 0xE8, 0xD9, 0x40, 0xAF, 0x0A, 0xD3 };

    UCHAR key_struct[258];
    UCHAR data[9];
    memcpy(data, plaintext, 9);

    XcRC4Key(key_struct, 3, key);
    XcRC4Crypt(key_struct, 9, data);

    BOOL matches = (memcmp(data, expected_ct, 9) == 0);
    GEN_CHECK(matches, TRUE, "RC4 encrypt matches known vector");

    // Decrypt: re-initialize key and decrypt
    XcRC4Key(key_struct, 3, key);
    XcRC4Crypt(key_struct, 9, data);

    BOOL decrypted = (memcmp(data, plaintext, 9) == 0);
    GEN_CHECK(decrypted, TRUE, "RC4 decrypt recovers plaintext");

    // --- Empty input ---
    XcRC4Key(key_struct, 3, key);
    UCHAR empty_buf[1] = { 0x42 };
    XcRC4Crypt(key_struct, 0, empty_buf);
    GEN_CHECK(empty_buf[0], 0x42, "zero-length no-op");

    TEST_END();
}
