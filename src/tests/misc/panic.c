#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(panic)
{
    TEST_BEGIN();

    // HalInitiateShutdown/panic is too dangerous to call
    // It would reboot/halt the system
    TEST_SKIP("would halt system");

    TEST_END();
}
