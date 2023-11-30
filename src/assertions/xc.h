#pragma once

#include <xboxkrnl/xboxkrnl.h>
#include <stddef.h>

BOOL assert_hashed_result_ex(
    PUCHAR input,
    size_t len,
    PUCHAR expected_result,
    const char *test_name,
    int line_number
);
#define assert_hashed_result( \
    input, \
    len, \
    expected_result, \
    test_name \
) \
assert_hashed_result_ex( \
    input, \
    len, \
    expected_result, \
    test_name, \
    __LINE__ \
)
