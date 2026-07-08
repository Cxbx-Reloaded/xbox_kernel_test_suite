#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KfRaiseIrql)
{
    TEST_BEGIN();

    // Should start at PASSIVE_LEVEL
    KIRQL current = KeGetCurrentIrql();
    GEN_CHECK(current, PASSIVE_LEVEL, "initial PASSIVE_LEVEL");

    // Raise from PASSIVE to APC_LEVEL
    KIRQL old_irql = KfRaiseIrql(APC_LEVEL);
    GEN_CHECK(old_irql, PASSIVE_LEVEL, "old irql from PASSIVE");
    current = KeGetCurrentIrql();
    GEN_CHECK(current, APC_LEVEL, "raised to APC_LEVEL");

    // Raise from APC to DISPATCH_LEVEL
    KIRQL old_irql2 = KfRaiseIrql(DISPATCH_LEVEL);
    GEN_CHECK(old_irql2, APC_LEVEL, "old irql from APC");
    current = KeGetCurrentIrql();
    GEN_CHECK(current, DISPATCH_LEVEL, "raised to DISPATCH_LEVEL");

    // Restore back down
    KfLowerIrql(APC_LEVEL);
    current = KeGetCurrentIrql();
    GEN_CHECK(current, APC_LEVEL, "lowered to APC_LEVEL");

    KfLowerIrql(PASSIVE_LEVEL);
    current = KeGetCurrentIrql();
    GEN_CHECK(current, PASSIVE_LEVEL, "lowered to PASSIVE_LEVEL");

    TEST_END();
}
