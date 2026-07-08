#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KiUnlockDispatcherDatabase)
{
    TEST_BEGIN();

    // KiUnlockDispatcherDatabase releases the dispatcher lock and lowers IRQL.
    // It's an internal kernel function that must be paired with acquiring the
    // dispatcher lock first (raising IRQL to DISPATCH_LEVEL).

    // Raise IRQL to DISPATCH_LEVEL (simulating dispatcher lock acquisition)
    KIRQL old_irql;
    old_irql = KfRaiseIrql(DISPATCH_LEVEL);

    // Call KiUnlockDispatcherDatabase to lower back
    KiUnlockDispatcherDatabase(old_irql);

    // Verify IRQL was lowered back
    KIRQL current = KeGetCurrentIrql();
    GEN_CHECK(current, old_irql, "IRQL restored");

    TEST_END();
}
