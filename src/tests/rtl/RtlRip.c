#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(RtlRip)
{
    TEST_BEGIN();

    // RtlRip outputs a debug message and optionally breaks into debugger.
    // On retail Xbox kernel, it should not crash.
    // Skip if debugger is enabled to avoid unwanted break.
    if (!KdDebuggerEnabled) {
        RtlRip("TestComponent", "TestMessage", "TestContext");
        // If we get here, the function returned without crashing
        GEN_CHECK(TRUE, TRUE, "RtlRip returned");
    } else {
        print("  SKIP: Debugger enabled, skipping RtlRip to avoid break");
    }

    TEST_END();
}
