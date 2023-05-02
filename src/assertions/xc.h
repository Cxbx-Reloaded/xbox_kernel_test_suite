#pragma once

#include <xboxkrnl/xboxkrnl.h>
#include <stddef.h>

BOOL assert_hashed_result(
    PUCHAR input,
    size_t len,
    PUCHAR expected_result,
    const char *test_name
);
