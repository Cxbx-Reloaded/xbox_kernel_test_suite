#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(AvSetSavedDataAddress)
{
    TEST_BEGIN();

    // Save original value
    PVOID original = AvGetSavedDataAddress();

    // Set to a known valid address (stack variable address as a safe pointer)
    DWORD dummy_data = 0x12345678;
    AvSetSavedDataAddress(&dummy_data);
    GEN_CHECK(AvGetSavedDataAddress(), (PVOID)&dummy_data, "set to stack address");

    // Set to a different address
    DWORD dummy_data2 = 0;
    AvSetSavedDataAddress(&dummy_data2);
    GEN_CHECK(AvGetSavedDataAddress(), (PVOID)&dummy_data2, "set to different address");

    // Restore original
    AvSetSavedDataAddress(original);
    GEN_CHECK(AvGetSavedDataAddress(), original, "restored original");

    TEST_END();
}
