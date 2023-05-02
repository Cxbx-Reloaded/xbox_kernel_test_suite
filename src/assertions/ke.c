#include "ke.h"
#include "defines.h"

BOOL assert_critical_region(
    PKTHREAD thread,
    ULONG expected_Apc,
    const char* test_name
) {
    ASSERT_HEADER

    GEN_CHECK(thread->KernelApcDisable, expected_Apc, "KernelApcDisable")

    ASSERT_FOOTER(test_name)
}
