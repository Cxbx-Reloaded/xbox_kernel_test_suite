#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(PhyInitialize)
{
    TEST_BEGIN();

    // SKIP: PhyInitialize resets and reinitializes the ethernet PHY hardware.
    // Calling with forceReset=TRUE would disrupt any active network connections
    // and may interfere with other tests or system stability.
    // The PHY is already initialized by the kernel at boot.

    // We can safely call without force reset to verify it doesn't crash
    NTSTATUS status = PhyInitialize(FALSE, NULL);
    BOOL valid = NT_SUCCESS(status);
    GEN_CHECK(valid, TRUE, "PhyInitialize(FALSE, NULL) succeeds");

    TEST_END();
}
