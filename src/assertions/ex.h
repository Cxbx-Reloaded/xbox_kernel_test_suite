#pragma once

#include <xboxkrnl/xboxkrnl.h>

BOOL assert_ERWLOCK_equals_ex(
    PERWLOCK,
    LONG,
    ULONG,
    ULONG,
    ULONG,
    const char*,
    int
);
#define assert_ERWLOCK_equals( \
    ReadWriteLock, \
    expected_LockCount, \
    expected_WritersWaitingCount, \
    expected_ReadersWaitingCount, \
    expected_ReadersEntryCount, \
    test_name \
) \
assert_ERWLOCK_equals_ex( \
    ReadWriteLock, \
    expected_LockCount, \
    expected_WritersWaitingCount, \
    expected_ReadersWaitingCount, \
    expected_ReadersEntryCount, \
    test_name, \
    __LINE__ \
)
