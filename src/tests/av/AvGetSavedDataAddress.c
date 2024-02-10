#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

void test_AvGetSavedDataAddress(int func_num, const char* func_name)
{
    // FIXME this test is broken - We shouldn't pass NULL to AvSetSavedDataAddress. This is just an hack to avoid a system crash.
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    PVOID address_old = AvGetSavedDataAddress();

    AvSetSavedDataAddress(NULL);
    GEN_CHECK(AvGetSavedDataAddress() == NULL, TRUE, "SavedDataAddress");

    AvSetSavedDataAddress(address_old);

    print_test_footer(func_num, func_name, test_passed);
}
