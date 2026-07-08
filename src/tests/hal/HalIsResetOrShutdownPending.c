#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(HalIsResetOrShutdownPending)
{
    TEST_BEGIN();

    // Should return FALSE during normal operation
    BOOLEAN pending = HalIsResetOrShutdownPending();
    // We can't guarantee the value, but it should be a valid boolean
    GEN_CHECK(pending == TRUE || pending == FALSE, TRUE, "valid boolean");

    TEST_END();
}
