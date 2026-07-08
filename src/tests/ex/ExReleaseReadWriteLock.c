#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ExReleaseReadWriteLock)
{
    TEST_BEGIN();

    ERWLOCK lock;
    ExInitializeReadWriteLock(&lock);

    // Acquire and release exclusive
    ExAcquireReadWriteLockExclusive(&lock);
    ExReleaseReadWriteLock(&lock);
    GEN_CHECK(lock.LockCount, -1, "exclusive release");

    // Acquire and release shared
    ExAcquireReadWriteLockShared(&lock);
    ExReleaseReadWriteLock(&lock);
    GEN_CHECK(lock.LockCount, -1, "shared release");

    TEST_END();
}
