#include <xboxkrnl/xboxkrnl.h>

#include "global.h"
#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(AvSetDisplayMode)
{
    TEST_BEGIN();

    // AvSetDisplayMode programs NV2A display registers directly.
    // Calling with valid parameters would change the active display mode,
    // corrupting video output for the test runner. Calling with invalid
    // parameters (NULL RegisterBase, 0 mode) crashes on both hardware and
    // emulators. Cannot be tested safely without a dedicated display mode
    // save/restore mechanism.

    TEST_SKIP("would corrupt display output");

    TEST_END();
}
