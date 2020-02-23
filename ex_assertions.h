#ifndef EX_ASSERTIONS_H
#define EX_ASSERTIONS_H

#include <xboxkrnl/xboxkrnl.h>

BOOL assert_ERWLOCK_equals(
    PERWLOCK,
    LONG,
    ULONG,
    ULONG,
    ULONG,
    const char*
);

#endif // EX_ASSERTIONS_H
