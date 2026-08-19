#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(AvSetDisplayMode)
{
    TEST_BEGIN();

    /* NOTE: The code below is an example that could break the hardware unintentionally.
    // FIXME: FATAL - Fails on emulator as of april 6th, 2018
    // FIXME: FATAL - Fails on real hardware too. The parameters passed to AvSetDisplayMode are wrong.
    AvSetDisplayMode(NULL, 0, 0, 0, 0, 0) == 0L ? print("0x0003 - AvSetDisplayMode: 0 (Good)") : print("0x0003 - AvSetDisplayMode: !=0 (Faulty)");
    */

    // AvSetDisplayMode programs NV2A display registers directly.
    // Calling with valid parameters would change the active display mode,
    // corrupting video output for the test runner. Calling with invalid
    // parameters (NULL RegisterBase, 0 mode) crashes on both hardware and
    // emulators. Cannot be tested safely without a dedicated display mode
    // save/restore mechanism.

    TEST_SKIP_END("will corrupt display output");
}
