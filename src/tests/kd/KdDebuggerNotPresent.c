#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KdDebuggerNotPresent)
{
    TEST_BEGIN();

    // KdDebuggerNotPresent is a BOOLEAN exported variable
    // It should be either TRUE or FALSE (valid boolean)
    BOOLEAN not_present = KdDebuggerNotPresent;
    GEN_CHECK(not_present == TRUE || not_present == FALSE, TRUE, "valid boolean value");

    // KdDebuggerNotPresent should be the logical opposite of KdDebuggerEnabled
    // (when debugger is enabled, it is present; when not present, it is not enabled)
    BOOLEAN enabled = KdDebuggerEnabled;
    if (enabled) {
        GEN_CHECK(not_present, FALSE, "debugger enabled means present");
    }

    TEST_END();
}
