#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KfLowerIrql)
{
    TEST_BEGIN();

    // Raise to DISPATCH_LEVEL
    KIRQL old_irql = KfRaiseIrql(DISPATCH_LEVEL);
    KIRQL current = KeGetCurrentIrql();
    GEN_CHECK(current, DISPATCH_LEVEL, "at DISPATCH_LEVEL");

    // Lower to APC_LEVEL
    KfLowerIrql(APC_LEVEL);
    current = KeGetCurrentIrql();
    GEN_CHECK(current, APC_LEVEL, "lowered to APC_LEVEL");

    // Lower to PASSIVE_LEVEL
    KfLowerIrql(PASSIVE_LEVEL);
    current = KeGetCurrentIrql();
    GEN_CHECK(current, PASSIVE_LEVEL, "lowered to PASSIVE_LEVEL");

    // Verify old_irql was PASSIVE_LEVEL
    GEN_CHECK(old_irql, PASSIVE_LEVEL, "original was PASSIVE_LEVEL");

    TEST_END();
}
