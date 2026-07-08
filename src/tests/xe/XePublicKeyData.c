#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(XePublicKeyData)
{
    TEST_BEGIN();

    // XePublicKeyData is a kernel-exported 284-byte array containing the
    // XBE public key used for signature verification.

    // XePublicKeyData is a fixed kernel export (array, not pointer) - always valid.
    // Verify accessible by reading first byte.
    volatile BYTE first_byte = XePublicKeyData[0];
    GEN_CHECK(first_byte == XePublicKeyData[0], TRUE, "XePublicKeyData accessible");

    // The public key data should not be all zeros (a valid key is always present)
    BOOL not_all_zeros = FALSE;
    for (ULONG i = 0; i < 284; i++) {
        if (XePublicKeyData[i] != 0) {
            not_all_zeros = TRUE;
            break;
        }
    }
    GEN_CHECK(not_all_zeros, TRUE, "key data not all zeros");

    // The public key should not be all 0xFF either (would indicate erased flash)
    BOOL not_all_ff = FALSE;
    for (ULONG i = 0; i < 284; i++) {
        if (XePublicKeyData[i] != 0xFF) {
            not_all_ff = TRUE;
            break;
        }
    }
    GEN_CHECK(not_all_ff, TRUE, "key data not all 0xFF");

    // Verify we can read all 284 bytes without faulting (memory is accessible)
    volatile ULONG checksum = 0;
    for (ULONG i = 0; i < 284; i++) {
        checksum += XePublicKeyData[i];
    }
    // Checksum should be > 0 given the not-all-zeros check above
    BOOL checksum_nonzero = (checksum > 0);
    GEN_CHECK(checksum_nonzero, TRUE, "checksum nonzero (readable)");

    TEST_END();
}
