#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmClaimGpuInstanceMemory)
{
    TEST_BEGIN();

    // MmClaimGpuInstanceMemory reserves memory for the NV2A GPU instance memory
    // It returns the physical address of the top of memory available after claim.
    // Calling with 0 should report current state without changing anything.
    PVOID instance_phys_addr = MmClaimGpuInstanceMemory(0, NULL);
    // Should return a non-zero value (top of available memory)
    GEN_CHECK(instance_phys_addr != 0, TRUE, "returns non-zero");

    TEST_END();
}
