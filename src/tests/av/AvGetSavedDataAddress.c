#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"

void test_AvGetSavedDataAddress()
{
    // FIXME this test is broken - We shouldn't pass NULL to AvSetSavedDataAddress. This is just an hack to avoid a system crash.
    const char* func_num = "0x0001";
    const char* func_name = "AvGetSavedDataAddress";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    AvSetSavedDataAddress(NULL);
    tests_passed = AvGetSavedDataAddress() == NULL ? 1 : 0;
    print("AvGetSavedDataAddress()=0x%x", AvGetSavedDataAddress());

    print_test_footer(func_num, func_name, tests_passed);
}
