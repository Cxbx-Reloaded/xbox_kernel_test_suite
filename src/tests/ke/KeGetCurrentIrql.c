#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeGetCurrentIrql)
{
    TEST_BEGIN();

    // --- At normal test execution: PASSIVE_LEVEL ---
    KIRQL irql = KeGetCurrentIrql();
    GEN_CHECK(irql, PASSIVE_LEVEL, "starts at PASSIVE_LEVEL");

    // --- After raise to DISPATCH_LEVEL ---
    KIRQL old = KfRaiseIrql(DISPATCH_LEVEL);
    irql = KeGetCurrentIrql();
    GEN_CHECK(irql, DISPATCH_LEVEL, "at DISPATCH_LEVEL");
    KfLowerIrql(old);

    // --- Back to PASSIVE after lower ---
    irql = KeGetCurrentIrql();
    GEN_CHECK(irql, PASSIVE_LEVEL, "back to PASSIVE");

    // --- Raise to higher levels ---
    old = KfRaiseIrql(DISPATCH_LEVEL);
    KIRQL old2 = KfRaiseIrql(PROFILE_LEVEL);
    irql = KeGetCurrentIrql();
    GEN_CHECK(irql, PROFILE_LEVEL, "at PROFILE_LEVEL");
    KfLowerIrql(old2);
    irql = KeGetCurrentIrql();
    GEN_CHECK(irql, DISPATCH_LEVEL, "back to DISPATCH");
    KfLowerIrql(old);
    irql = KeGetCurrentIrql();
    GEN_CHECK(irql, PASSIVE_LEVEL, "back to PASSIVE after nested");

    // --- Consistency: multiple rapid calls ---
    for (ULONG i = 0; i < 10; i++) {
        GEN_CHECK(KeGetCurrentIrql(), PASSIVE_LEVEL, "consistent PASSIVE");
    }

    // --- APC_LEVEL ---
    old = KfRaiseIrql(APC_LEVEL);
    irql = KeGetCurrentIrql();
    GEN_CHECK(irql, APC_LEVEL, "at APC_LEVEL");
    KfLowerIrql(old);

    TEST_END();
}
