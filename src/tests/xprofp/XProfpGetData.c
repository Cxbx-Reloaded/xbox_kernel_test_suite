#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(XProfpGetData)
{
    TEST_BEGIN();

    // XProfpGetData retrieves profiling data, only available in debug kernels.

    TEST_SKIP_END("debug kernel only, no prototype available");
}
