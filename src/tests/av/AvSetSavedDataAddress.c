#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"

TEST_FUNC(AvSetSavedDataAddress)
{
    // FIXME this test is broken - We shouldn't pass NULL to AvSetSavedDataAddress. This is just an hack to avoid a system crash.
    TEST_BEGIN();

    AvSetSavedDataAddress((void*) NULL);
    test_passed = AvGetSavedDataAddress()==(void*) NULL ? 1 : 0;

    TEST_END();
}
