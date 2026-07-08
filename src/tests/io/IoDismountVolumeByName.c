#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(IoDismountVolumeByName)
{
    TEST_BEGIN();

    // IoDismountVolumeByName unmounts a volume by its device name string.
    // Same safety concerns as IoDismountVolume.

    TEST_SKIP("would unmount real volume");

    TEST_END();
}
