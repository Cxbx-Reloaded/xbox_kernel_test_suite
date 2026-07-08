#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeInitializeSemaphore)
{
    TEST_BEGIN();

    // --- Count=2, Limit=5 ---
    KSEMAPHORE sem;
    memset(&sem, 0xCC, sizeof(sem));
    KeInitializeSemaphore(&sem, 2, 5);
    GEN_CHECK(sem.Header.SignalState, 2, "count=2");
    GEN_CHECK(sem.Limit, 5, "limit=5");
    GEN_CHECK(sem.Header.Type, SemaphoreObject, "type is SemaphoreObject");

    // Wait list initialized
    GEN_CHECK(sem.Header.WaitListHead.Flink, &sem.Header.WaitListHead, "waitlist flink");
    GEN_CHECK(sem.Header.WaitListHead.Blink, &sem.Header.WaitListHead, "waitlist blink");

    // --- Count=0 (unsignaled, waiters would block) ---
    KeInitializeSemaphore(&sem, 0, 10);
    GEN_CHECK(sem.Header.SignalState, 0, "count=0");
    GEN_CHECK(sem.Limit, 10, "limit=10");

    // Poll should timeout
    LARGE_INTEGER zero = {0};
    NTSTATUS status = KeWaitForSingleObject(&sem, Executive, KernelMode, FALSE, &zero);
    GEN_CHECK(status, STATUS_TIMEOUT, "count=0 timeout");

    // --- Count=limit (fully available) ---
    KeInitializeSemaphore(&sem, 5, 5);
    GEN_CHECK(sem.Header.SignalState, 5, "count==limit");
    // Should be immediately satisfiable
    status = KeWaitForSingleObject(&sem, Executive, KernelMode, FALSE, &zero);
    GEN_CHECK(status, STATUS_SUCCESS, "count>0 immediate");
    GEN_CHECK(sem.Header.SignalState, 4, "count decremented to 4");

    // --- Count=1, Limit=1 (binary semaphore / mutex-like) ---
    KeInitializeSemaphore(&sem, 1, 1);
    GEN_CHECK(sem.Header.SignalState, 1, "binary sem count=1");
    GEN_CHECK(sem.Limit, 1, "binary sem limit=1");
    status = KeWaitForSingleObject(&sem, Executive, KernelMode, FALSE, &zero);
    GEN_CHECK(status, STATUS_SUCCESS, "binary acquire");
    GEN_CHECK(sem.Header.SignalState, 0, "binary now 0");
    status = KeWaitForSingleObject(&sem, Executive, KernelMode, FALSE, &zero);
    GEN_CHECK(status, STATUS_TIMEOUT, "binary blocked");

    // Release back
    KeReleaseSemaphore(&sem, IO_NO_INCREMENT, 1, FALSE);
    GEN_CHECK(sem.Header.SignalState, 1, "binary released");

    // --- Large limit ---
    KeInitializeSemaphore(&sem, 100, 1000);
    GEN_CHECK(sem.Header.SignalState, 100, "large count");
    GEN_CHECK(sem.Limit, 1000, "large limit");

    TEST_END();
}
