#include <xboxkrnl/xboxkrnl.h>

#include "global.h" // for NV2A_MMIO_BASE var
#include "util/output.h"
#include "assertions/defines.h"

// AV capability bitmask definitions
#define AV_PACK_MASK 0x000000FF

TEST_FUNC(AvSendTVEncoderOption)
{
    TEST_BEGIN();

    // Query AV capabilities (option 6 = AV_QUERY_AV_CAPABILITIES)
    // Returns a bitmask: pack type in low byte, region/flags in upper bits
    ULONG result = 0;
    AvSendTVEncoderOption((void *)NV2A_MMIO_BASE, 6, 0, &result);

    // Extract pack type from capabilities bitmask
    ULONG pack_type = result & AV_PACK_MASK;
    GEN_CHECK(pack_type <= 6, TRUE, "AV pack type in valid range");

    // Overall capabilities should be non-zero (at minimum, region flags present)
    GEN_CHECK(result != 0, TRUE, "capabilities non-zero");

    // Query again to verify consistency
    ULONG result2 = 0;
    AvSendTVEncoderOption((void *)NV2A_MMIO_BASE, 6, 0, &result2);
    GEN_CHECK(result2, result, "consistent result on repeated query");

    TEST_END();
}
