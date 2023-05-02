#pragma once

#include <xboxkrnl/xboxkrnl.h>

BOOL assert_ERWLOCK_equals(
    PERWLOCK,
    LONG,
    ULONG,
    ULONG,
    ULONG,
    const char*
);
