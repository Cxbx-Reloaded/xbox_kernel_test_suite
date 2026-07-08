#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeDisconnectInterrupt)
{
    TEST_BEGIN();

    // Disconnecting active interrupts is dangerous

    TEST_SKIP("dangerous - could disconnect active IRQ");

    TEST_END();
}
