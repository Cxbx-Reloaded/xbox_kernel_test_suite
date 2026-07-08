#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(IoDismountVolume)
{
    TEST_BEGIN();

    // IoDismountVolume unmounts a filesystem volume from a device object.
    // Calling this on any mounted volume would unmount it, potentially
    // making the system unstable or preventing further file I/O.

    TEST_SKIP("would unmount real volume");

    TEST_END();
}
