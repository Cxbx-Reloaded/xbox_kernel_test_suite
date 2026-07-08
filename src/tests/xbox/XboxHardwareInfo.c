#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(XboxHardwareInfo)
{
    TEST_BEGIN();

    // XboxHardwareInfo is an exported XBOX_HARDWARE_INFO structure
    // Flags is a bitmask: 1=USB daughterboard, 2=devkit, 4=focus encoder, 8=chihiro
    // A retail system without focus encoder or USB daughterboard will have Flags == 0
    GEN_CHECK((XboxHardwareInfo.Flags & ~0xF) == 0, TRUE, "Flags only known bits set");
    // GpuRevision should be a valid NV2A revision
    GEN_CHECK(XboxHardwareInfo.GpuRevision != 0, TRUE, "GpuRevision non-zero");

    TEST_END();
}
