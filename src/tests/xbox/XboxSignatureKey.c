#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(XboxSignatureKey)
{
    TEST_BEGIN();

    // XboxSignatureKey is a 16-byte exported array
    BOOLEAN is_valid = is_kernel_export_valid((PVOID)XboxSignatureKey);
    GEN_CHECK(is_valid, TRUE, "SignatureKey address valid");

    TEST_END();
}
