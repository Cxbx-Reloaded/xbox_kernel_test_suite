#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeRaiseIrqlToSynchLevel)
{
    TEST_BEGIN();

    // Should start at PASSIVE_LEVEL
    KIRQL current = KeGetCurrentIrql();
    GEN_CHECK(current, PASSIVE_LEVEL, "initial PASSIVE_LEVEL");

    // Raise to synch level (DISPATCH_LEVEL + 1 on Xbox = SYNCH_LEVEL = 28)
    KIRQL old_irql = KeRaiseIrqlToSynchLevel();
    GEN_CHECK(old_irql, PASSIVE_LEVEL, "old irql");

    current = KeGetCurrentIrql();
    // SYNCH_LEVEL is typically DISPATCH_LEVEL on single-proc Xbox
    GEN_CHECK((UCHAR)(current >= DISPATCH_LEVEL), TRUE, "at or above DISPATCH_LEVEL");

    // Restore
    KfLowerIrql(old_irql);
    current = KeGetCurrentIrql();
    GEN_CHECK(current, PASSIVE_LEVEL, "restored to PASSIVE_LEVEL");

    TEST_END();
}
