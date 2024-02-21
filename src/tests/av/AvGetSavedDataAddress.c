#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(AvGetSavedDataAddress)
{
    TEST_BEGIN();

    // FIXME this test is broken - We shouldn't pass NULL to AvSetSavedDataAddress. This is just an hack to avoid a system crash.

    PVOID address_old = AvGetSavedDataAddress();

    AvSetSavedDataAddress(NULL);
    GEN_CHECK(AvGetSavedDataAddress() == NULL, TRUE, "SavedDataAddress");

    AvSetSavedDataAddress(address_old);

    TEST_END();
}
