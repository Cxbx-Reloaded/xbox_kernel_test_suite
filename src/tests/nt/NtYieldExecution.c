#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtYieldExecution)
{
    TEST_BEGIN();

    // NtYieldExecution yields the remainder of the current thread's time slice.
    // Returns STATUS_NO_YIELD_PERFORMED if no other thread is ready to run,
    // or STATUS_SUCCESS if a context switch occurred.

    NTSTATUS status = NtYieldExecution();
    // Either result is valid depending on system load
    BOOL valid_result = (status == STATUS_SUCCESS || status == STATUS_NO_YIELD_PERFORMED);
    GEN_CHECK(valid_result, TRUE, "valid return code");

    // Multiple calls should not crash and should return valid codes
    for (ULONG i = 0; i < 10; i++) {
        status = NtYieldExecution();
        valid_result = (status == STATUS_SUCCESS || status == STATUS_NO_YIELD_PERFORMED);
        GEN_CHECK(valid_result, TRUE, "repeated yield valid");
    }

    TEST_END();
}
