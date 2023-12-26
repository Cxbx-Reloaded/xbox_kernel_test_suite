#include "common.h"
#include "defines.h"

BOOL assert_NTSTATUS_ex(
    NTSTATUS status,
    NTSTATUS expected_status,
    const char* func_name,
    int line_number)
{
    ASSERT_HEADER
    if(status != expected_status) {
        print(
            "  ERROR(line %d): Expected return status of function '%s' = 0x%x, got = 0x%x",
            line_number,
            func_name,
            expected_status,
            status
        );
        test_passed = 0;
    }
    return test_passed;
}
