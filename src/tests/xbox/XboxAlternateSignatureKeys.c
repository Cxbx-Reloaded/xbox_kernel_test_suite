#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(XboxAlternateSignatureKeys)
{
    TEST_BEGIN();

    // XboxAlternateSignatureKeys is an array of 16 alternate 16-byte keys
    BOOLEAN is_valid = is_kernel_export_valid((PVOID)XboxAlternateSignatureKeys);
    GEN_CHECK(is_valid, TRUE, "AlternateSignatureKeys address valid");

    TEST_END();
}
