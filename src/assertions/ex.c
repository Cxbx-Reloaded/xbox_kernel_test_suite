#include <xboxkrnl/xboxkrnl.h>

#include "defines.h"

BOOL assert_ERWLOCK_equals(
    PERWLOCK ReadWriteLock,
    LONG expected_LockCount,
    ULONG expected_WritersWaitingCount,
    ULONG expected_ReadersWaitingCount,
    ULONG expected_ReadersEntryCount,
    const char* test_name)
{
    ASSERT_HEADER

    GEN_CHECK(ReadWriteLock->LockCount, expected_LockCount, "LockCount")
    GEN_CHECK(ReadWriteLock->WritersWaitingCount, expected_WritersWaitingCount, "WritersWaitingCount")
    GEN_CHECK(ReadWriteLock->ReadersWaitingCount, expected_ReadersWaitingCount, "ReadersWaitingCount")
    GEN_CHECK(ReadWriteLock->ReadersEntryCount, expected_ReadersEntryCount, "ReadersEntryCount")

    ASSERT_FOOTER(test_name)
}
