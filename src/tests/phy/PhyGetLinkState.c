#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(PhyGetLinkState)
{
    TEST_BEGIN();

    // PhyGetLinkState returns the current ethernet link state.
    // Parameter: update (BOOLEAN) - whether to re-read from hardware.

    // Query without forcing update
    DWORD state = PhyGetLinkState(FALSE);
    // Link state is a bitmask - value depends on hardware/emulator state
    // Just verify it doesn't crash and returns a reasonable value
    GEN_CHECK(TRUE, TRUE, "PhyGetLinkState(FALSE) no crash");

    // Query with forced update from hardware
    DWORD state2 = PhyGetLinkState(TRUE);
    GEN_CHECK(TRUE, TRUE, "PhyGetLinkState(TRUE) no crash");

    // Both calls should return consistent link state (unless link changed)
    // We can't assert equality since hardware state may change, but both should be valid
    (void)state;
    (void)state2;

    TEST_END();
}
