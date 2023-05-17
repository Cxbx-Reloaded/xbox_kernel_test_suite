#pragma once

#include <xboxkrnl/xboxkrnl.h>

BOOL assert_NTSTATUS(
    NTSTATUS,
    NTSTATUS,
    const char*
);
