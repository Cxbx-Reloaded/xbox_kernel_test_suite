#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"

void test_AvSetSavedDataAddress(int func_num, const char* func_name)
{
    // FIXME this test is broken - We shouldn't pass NULL to AvSetSavedDataAddress. This is just an hack to avoid a system crash.
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    AvSetSavedDataAddress((void*) NULL);
    tests_passed = AvGetSavedDataAddress()==(void*) NULL ? 1 : 0;

    print_test_footer(func_num, func_name, tests_passed);
}
