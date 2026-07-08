#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(HalDisableSystemInterrupt)
{
    TEST_BEGIN();

    // Disabling and re-enabling a system interrupt is dangerous

    TEST_SKIP("dangerous - could disable critical system IRQ");

    TEST_END();
}
