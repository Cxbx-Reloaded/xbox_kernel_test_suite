#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(HalClearSoftwareInterrupt)
{
    TEST_BEGIN();

    // Clear APC software interrupt - should be safe when none pending
    HalClearSoftwareInterrupt(APC_LEVEL);
    GEN_CHECK(TRUE, TRUE, "clear APC interrupt no crash");

    TEST_END();
}
