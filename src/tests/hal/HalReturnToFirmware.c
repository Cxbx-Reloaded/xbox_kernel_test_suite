#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(HalReturnToFirmware)
{
    TEST_BEGIN();

    // HalReturnToFirmware will reboot/shutdown
    // TODO: Need a config variable to determine allow to test or not

    TEST_SKIP_END("will shut down, or reboot, system");
}
