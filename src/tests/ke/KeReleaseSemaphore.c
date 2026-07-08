#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeReleaseSemaphore)
{
    TEST_BEGIN();

    KSEMAPHORE sem;
    KeInitializeSemaphore(&sem, 0, 5); // count=0, limit=5

    // --- Release by 1, returns previous count ---
    LONG prev = KeReleaseSemaphore(&sem, IO_NO_INCREMENT, 1, FALSE);
    GEN_CHECK(prev, 0, "prev count 0");
    GEN_CHECK(sem.Header.SignalState, 1, "count now 1");

    // --- Release by 2 ---
    prev = KeReleaseSemaphore(&sem, IO_NO_INCREMENT, 2, FALSE);
    GEN_CHECK(prev, 1, "prev count 1");
    GEN_CHECK(sem.Header.SignalState, 3, "count now 3");

    // --- Release to exactly limit ---
    prev = KeReleaseSemaphore(&sem, IO_NO_INCREMENT, 2, FALSE);
    GEN_CHECK(prev, 3, "prev count 3");
    GEN_CHECK(sem.Header.SignalState, 5, "count now 5 (at limit)");

    // --- Release beyond limit raises exception ---
    BOOLEAN caught = FALSE;
    __try {
        KeReleaseSemaphore(&sem, IO_NO_INCREMENT, 1, FALSE);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        caught = TRUE;
    }
    GEN_CHECK(caught, TRUE, "over-release raises exception");
    // Count should remain at limit
    GEN_CHECK(sem.Header.SignalState, 5, "count unchanged after over-release");

    // --- Acquire then release cycle ---
    KeInitializeSemaphore(&sem, 3, 3);
    LARGE_INTEGER zero = {0};
    KeWaitForSingleObject(&sem, Executive, KernelMode, FALSE, &zero); // 3->2
    KeWaitForSingleObject(&sem, Executive, KernelMode, FALSE, &zero); // 2->1
    GEN_CHECK(sem.Header.SignalState, 1, "after 2 waits: count=1");

    prev = KeReleaseSemaphore(&sem, IO_NO_INCREMENT, 2, FALSE);
    GEN_CHECK(prev, 1, "prev after waits");
    GEN_CHECK(sem.Header.SignalState, 3, "restored to limit");

    // --- Release with Adjustment=0 ---
    // SKIP: Releasing with Adjustment=0 is unusual but technically valid.
    // Returns current count without modifying. However, some implementations
    // may not support it. Skipping for portability.

    TEST_END();
}
