#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(AvGetSavedDataAddress)
{
    TEST_BEGIN();

    // Save original value
    PVOID original = AvGetSavedDataAddress();

    // Set to a known address and verify get returns it
    DWORD dummy_data = 0xABCDEF01;
    AvSetSavedDataAddress(&dummy_data);
    GEN_CHECK(AvGetSavedDataAddress(), (PVOID)&dummy_data, "get matches set");

    // Verify consistency (calling get multiple times returns same value)
    GEN_CHECK(AvGetSavedDataAddress(), AvGetSavedDataAddress(), "consistent");

    // Restore original
    AvSetSavedDataAddress(original);
    GEN_CHECK(AvGetSavedDataAddress(), original, "restored");

    TEST_END();
}
