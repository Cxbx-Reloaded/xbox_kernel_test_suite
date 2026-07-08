#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(XboxKrnlVersion)
{
    TEST_BEGIN();

    // XboxKrnlVersion is an XBOX_KRNL_VERSION struct
    // with Major, Minor, Build, Qfe fields
    // Major should be 1 for original Xbox
    GEN_CHECK(XboxKrnlVersion.Major, 1, "Major");
    // Minor should be 0
    GEN_CHECK(XboxKrnlVersion.Minor, 0, "Minor");
    // Build should be a reasonable kernel build number (3000-6000 range)
    GEN_CHECK(XboxKrnlVersion.Build >= 3000, TRUE, "Build >= 3000");
    GEN_CHECK(XboxKrnlVersion.Build <= 6000, TRUE, "Build <= 6000");

    TEST_END();
}
