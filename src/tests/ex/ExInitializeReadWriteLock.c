#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ExInitializeReadWriteLock)
{
    TEST_BEGIN();

    // --- Basic initialization ---
    ERWLOCK lock;
    // Pre-fill with garbage to verify all fields are set
    memset(&lock, 0xCC, sizeof(lock));

    ExInitializeReadWriteLock(&lock);

    GEN_CHECK(lock.LockCount, -1, "LockCount (unlocked = -1)");
    GEN_CHECK(lock.WritersWaitingCount, 0, "WritersWaitingCount == 0");
    GEN_CHECK(lock.ReadersWaitingCount, 0, "ReadersWaitingCount == 0");
    GEN_CHECK(lock.ReadersEntryCount, 0, "ReadersEntryCount == 0");

    // --- Acquire exclusive, verify state, release, verify unlocked again ---
    ExAcquireReadWriteLockExclusive(&lock);
    GEN_CHECK(lock.LockCount, 0, "LockCount after exclusive acquire");
    GEN_CHECK(lock.WritersWaitingCount, 0, "no writers waiting (we have it)");

    ExReleaseReadWriteLock(&lock);
    GEN_CHECK(lock.LockCount, -1, "LockCount back to -1 after release");

    // --- Acquire shared, verify state, release ---
    ExAcquireReadWriteLockShared(&lock);
    GEN_CHECK(lock.LockCount, 0, "LockCount after shared acquire");
    GEN_CHECK(lock.ReadersEntryCount, 1, "ReadersEntryCount == 1");

    ExReleaseReadWriteLock(&lock);
    GEN_CHECK(lock.LockCount, -1, "LockCount after shared release");
    GEN_CHECK(lock.ReadersEntryCount, 0, "ReadersEntryCount back to 0");

    // --- Re-initialization resets all state ---
    ExAcquireReadWriteLockExclusive(&lock);
    ExInitializeReadWriteLock(&lock); // re-init while "held"
    GEN_CHECK(lock.LockCount, -1, "re-init resets LockCount");
    GEN_CHECK(lock.ReadersEntryCount, 0, "re-init resets ReadersEntryCount");
    GEN_CHECK(lock.WritersWaitingCount, 0, "re-init resets WritersWaiting");
    GEN_CHECK(lock.ReadersWaitingCount, 0, "re-init resets ReadersWaiting");

    // --- Multiple distinct locks are independent ---
    ERWLOCK lock2;
    ExInitializeReadWriteLock(&lock2);
    ExAcquireReadWriteLockExclusive(&lock);
    GEN_CHECK(lock.LockCount, 0, "lock1 acquired");
    GEN_CHECK(lock2.LockCount, -1, "lock2 still unlocked");
    ExReleaseReadWriteLock(&lock);

    TEST_END();
}
