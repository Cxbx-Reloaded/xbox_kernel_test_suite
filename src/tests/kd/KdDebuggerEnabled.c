#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KdDebuggerEnabled)
{
    TEST_BEGIN();

    // KdDebuggerEnabled is a BOOLEAN exported variable
    // It should be either TRUE or FALSE (valid boolean)
    BOOLEAN enabled = KdDebuggerEnabled;
    GEN_CHECK(enabled == TRUE || enabled == FALSE, TRUE, "valid boolean value");

    TEST_END();
}
