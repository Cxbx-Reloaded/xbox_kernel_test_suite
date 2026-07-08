#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(HalEnableSystemInterrupt)
{
    TEST_BEGIN();

    // Enabling system interrupts for arbitrary vectors is dangerous

    TEST_SKIP("dangerous - could enable unexpected IRQ");

    TEST_END();
}
