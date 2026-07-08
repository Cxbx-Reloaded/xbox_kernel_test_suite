#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(XcUpdateCrypto)
{
    TEST_BEGIN();

    // XcUpdateCrypto replaces the kernel's crypto function vector table.
    // It returns the current (ROM) vector in pROMVector.
    // CAUTION: We must restore the original vector after testing.

    // Get the current ROM vector
    CRYPTO_VECTOR rom_vector;
    memset(&rom_vector, 0, sizeof(rom_vector));
    XcUpdateCrypto(NULL, &rom_vector);

    // ROM vector should have valid function pointers
    GEN_CHECK(rom_vector.pXcSHAInit != NULL, TRUE, "ROM SHAInit not NULL");
    GEN_CHECK(rom_vector.pXcSHAUpdate != NULL, TRUE, "ROM SHAUpdate not NULL");
    GEN_CHECK(rom_vector.pXcSHAFinal != NULL, TRUE, "ROM SHAFinal not NULL");
    GEN_CHECK(rom_vector.pXcRC4Key != NULL, TRUE, "ROM RC4Key not NULL");
    GEN_CHECK(rom_vector.pXcRC4Crypt != NULL, TRUE, "ROM RC4Crypt not NULL");
    GEN_CHECK(rom_vector.pXcHMAC != NULL, TRUE, "ROM HMAC not NULL");

    // SKIP: Actually replacing the crypto vector with custom functions would
    // affect all subsequent crypto operations in the kernel. This is dangerous
    // in a test environment. We only verify we can read the current vector.

    TEST_END();
}
