#ifndef COMMON_ASSERTIONS_H
#define COMMON_ASSERTIONS_H

#include <xboxkrnl/xboxkrnl.h>

BOOL assert_NTSTATUS(
    NTSTATUS,
    NTSTATUS,
    const char*
);

#endif // COMMON_ASSERTIONS_H
