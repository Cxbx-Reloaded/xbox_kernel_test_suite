#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(RtlAssert)
{
    TEST_BEGIN();

    // RtlAssert calls DbgPrompt in debug builds, or does nothing in retail.
    // On retail Xbox kernel, calling RtlAssert should not crash.
    // We cannot verify side effects (debug output) but can verify it returns.
    // NOTE: On debug kernels with a debugger attached, this would break into
    // the debugger. On retail/emulator this is a no-op.
    if (!KdDebuggerEnabled) {
        RtlAssert("test assertion", "test_RtlAssert", 1, NULL);
        // If we get here, the function returned without crashing
        GEN_CHECK(TRUE, TRUE, "RtlAssert returned");
    } else {
        // Skip if debugger is active to avoid breaking
        print("  SKIP: Debugger enabled, skipping RtlAssert to avoid break");
    }

    TEST_END();
}
