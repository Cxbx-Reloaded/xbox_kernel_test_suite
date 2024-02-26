#pragma once

#include "defines.h"

#define assert_ERWLOCK_equals_ex( \
    ReadWriteLock, \
    expected_LockCount, \
    expected_WritersWaitingCount, \
    expected_ReadersWaitingCount, \
    expected_ReadersEntryCount, \
    line_number \
) \
    GEN_CHECK_EX((ReadWriteLock)->LockCount, expected_LockCount, ".LockCount", line_number) \
    GEN_CHECK_EX((ReadWriteLock)->WritersWaitingCount, expected_WritersWaitingCount, ".WritersWaitingCount", line_number) \
    GEN_CHECK_EX((ReadWriteLock)->ReadersWaitingCount, expected_ReadersWaitingCount, ".ReadersWaitingCount", line_number) \
    GEN_CHECK_EX((ReadWriteLock)->ReadersEntryCount, expected_ReadersEntryCount, ".ReadersEntryCount", line_number)
#define assert_ERWLOCK_equals( \
    ReadWriteLock, \
    expected_LockCount, \
    expected_WritersWaitingCount, \
    expected_ReadersWaitingCount, \
    expected_ReadersEntryCount \
) \
assert_ERWLOCK_equals_ex( \
    ReadWriteLock, \
    expected_LockCount, \
    expected_WritersWaitingCount, \
    expected_ReadersWaitingCount, \
    expected_ReadersEntryCount, \
    __LINE__ \
)
