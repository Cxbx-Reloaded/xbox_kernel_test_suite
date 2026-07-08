#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(XboxLANKey)
{
    TEST_BEGIN();

    // XboxLANKey is a 16-byte exported array (LAN key)
    BOOLEAN is_valid = is_kernel_export_valid((PVOID)XboxLANKey);
    GEN_CHECK(is_valid, TRUE, "LANKey address valid");

    TEST_END();
}
