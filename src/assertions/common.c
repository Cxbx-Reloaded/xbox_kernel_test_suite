#include "common.h"
#include "defines.h"

BOOL assert_NTSTATUS(
    NTSTATUS status,
    NTSTATUS expected_status,
    const char* func_name
) {
    ASSERT_HEADER
    if(status != expected_status) {
        print(
            "  Expected return status of function '%s' = 0x%x, got = 0x%x",
            func_name,
            expected_status,
            status
        );
        test_passed = 0;
    }
    else {
        print(
            "  Function '%s' returned 0x%x as expected",
            func_name,
            status
        );
    }
    return test_passed;
}
