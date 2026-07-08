#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(HalBootSMCVideoMode)
{
    TEST_BEGIN();

    // HalBootSMCVideoMode is an exported ULONG containing the video mode
    // set by the SMC at boot time. Valid values: 0-6 typically.
    // 0=SCART, 1=HDTV, 2=VGA, 3=RFU, 4=SVIDEO, 5=COMPOSITE, 6=VGA_SOG
    GEN_CHECK(HalBootSMCVideoMode <= 6, TRUE, "valid video mode range");

    TEST_END();
}
