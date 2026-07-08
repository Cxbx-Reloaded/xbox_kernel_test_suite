#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(HalEnableSecureTrayEject)
{
    TEST_BEGIN();

    // HalEnableSecureTrayEject prevents the user from ejecting
    // the tray without software permission. Safe to call.
    HalEnableSecureTrayEject();
    GEN_CHECK(TRUE, TRUE, "enable secure eject no crash");

    TEST_END();
}
