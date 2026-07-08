#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeRaiseIrqlToDpcLevel)
{
    TEST_BEGIN();

    // --- Start at PASSIVE_LEVEL ---
    KIRQL current = KeGetCurrentIrql();
    GEN_CHECK(current, PASSIVE_LEVEL, "initial PASSIVE_LEVEL");

    // --- Raise to DPC level ---
    KIRQL old_irql = KeRaiseIrqlToDpcLevel();
    GEN_CHECK(old_irql, PASSIVE_LEVEL, "old irql was PASSIVE");
    current = KeGetCurrentIrql();
    GEN_CHECK(current, DISPATCH_LEVEL, "now at DISPATCH_LEVEL");

    // --- Calling again while already at DPC: returns DISPATCH_LEVEL ---
    KIRQL old_irql2 = KeRaiseIrqlToDpcLevel();
    GEN_CHECK(old_irql2, DISPATCH_LEVEL, "already at DISPATCH");
    current = KeGetCurrentIrql();
    GEN_CHECK(current, DISPATCH_LEVEL, "still DISPATCH_LEVEL");

    // Lower back (from the double-raise)
    KfLowerIrql(old_irql2);
    current = KeGetCurrentIrql();
    GEN_CHECK(current, DISPATCH_LEVEL, "back to DISPATCH after inner lower");

    // Lower to original
    KfLowerIrql(old_irql);
    current = KeGetCurrentIrql();
    GEN_CHECK(current, PASSIVE_LEVEL, "restored to PASSIVE_LEVEL");

    // --- Raise/lower cycle is repeatable ---
    for (ULONG i = 0; i < 5; i++) {
        KIRQL oi = KeRaiseIrqlToDpcLevel();
        GEN_CHECK(KeGetCurrentIrql(), DISPATCH_LEVEL, "loop: at DISPATCH");
        KfLowerIrql(oi);
        GEN_CHECK(KeGetCurrentIrql(), PASSIVE_LEVEL, "loop: at PASSIVE");
    }

    // --- At DISPATCH_LEVEL, thread rescheduling is deferred ---
    // Can't easily verify this without another thread, but we can verify
    // basic operations still work at DISPATCH
    old_irql = KeRaiseIrqlToDpcLevel();
    volatile ULONG x = 42;
    GEN_CHECK(x, 42, "computation works at DISPATCH");
    KfLowerIrql(old_irql);

    TEST_END();
}
