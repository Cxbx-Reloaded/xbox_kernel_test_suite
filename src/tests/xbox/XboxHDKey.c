#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(XboxHDKey)
{
    TEST_BEGIN();

    // XboxHDKey is a 16-byte exported array (hard drive key)
    BOOLEAN is_valid = is_kernel_export_valid((PVOID)XboxHDKey);
    GEN_CHECK(is_valid, TRUE, "HDKey address valid");

    TEST_END();
}
