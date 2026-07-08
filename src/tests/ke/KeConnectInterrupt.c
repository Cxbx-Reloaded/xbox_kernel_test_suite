#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeConnectInterrupt)
{
    TEST_BEGIN();

    // Connecting interrupts to active IRQs is dangerous

    TEST_SKIP("dangerous - could steal IRQ");

    TEST_END();
}
