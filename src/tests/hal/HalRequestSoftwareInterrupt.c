#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(HalRequestSoftwareInterrupt)
{
    TEST_BEGIN();

    // Request and immediately clear an APC-level software interrupt
    // Raise IRQL first to prevent immediate delivery
    KIRQL old_irql;
    old_irql = KfRaiseIrql(DISPATCH_LEVEL);

    HalRequestSoftwareInterrupt(APC_LEVEL);
    HalClearSoftwareInterrupt(APC_LEVEL);

    KfLowerIrql(old_irql);
    GEN_CHECK(TRUE, TRUE, "request+clear no crash");

    TEST_END();
}
