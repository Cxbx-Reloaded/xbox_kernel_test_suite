#include "ke.h"
#include "defines.h"

BOOL assert_critical_region_ex(
    PKTHREAD thread,
    ULONG expected_Apc,
    const char* test_name,
    int line_number)
{
    ASSERT_HEADER

    GEN_CHECK_EX(thread->KernelApcDisable, expected_Apc, "KernelApcDisable", line_number)

    ASSERT_FOOTER(test_name)
}
