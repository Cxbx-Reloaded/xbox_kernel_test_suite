#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(HalInitiateShutdown)
{
    TEST_BEGIN();

    // HalInitiateShutdown will shut down the system
    // TODO: Need a config variable to determine allow to test or not

    TEST_SKIP_END("will shut down system");
}
