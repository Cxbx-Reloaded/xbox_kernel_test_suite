#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(XboxEEPROMKey)
{
    TEST_BEGIN();

    // XboxEEPROMKey is a 16-byte exported array used for EEPROM encryption
    // Verify the pointer is valid and in kernel space
    BOOLEAN is_valid = is_kernel_export_valid((PVOID)XboxEEPROMKey);
    GEN_CHECK(is_valid, TRUE, "EEPROMKey address valid");

    TEST_END();
}
