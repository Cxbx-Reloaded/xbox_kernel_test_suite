#include <xboxkrnl/xboxkrnl.h>

#include "defines.h"

BOOL assert_ERWLOCK_equals_ex(
    PERWLOCK ReadWriteLock,
    LONG expected_LockCount,
    ULONG expected_WritersWaitingCount,
    ULONG expected_ReadersWaitingCount,
    ULONG expected_ReadersEntryCount,
    const char* test_name,
    int line_number)
{
    ASSERT_HEADER

    GEN_CHECK_EX(ReadWriteLock->LockCount, expected_LockCount, "LockCount", line_number)
    GEN_CHECK_EX(ReadWriteLock->WritersWaitingCount, expected_WritersWaitingCount, "WritersWaitingCount", line_number)
    GEN_CHECK_EX(ReadWriteLock->ReadersWaitingCount, expected_ReadersWaitingCount, "ReadersWaitingCount", line_number)
    GEN_CHECK_EX(ReadWriteLock->ReadersEntryCount, expected_ReadersEntryCount, "ReadersEntryCount", line_number)

    ASSERT_FOOTER(test_name)
}
