#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(XProfpControl)
{
    TEST_BEGIN();

    // XProfpControl is a profiling control API, only available in debug kernels.

    TEST_SKIP_END("debug kernel only, no prototype available");
}
