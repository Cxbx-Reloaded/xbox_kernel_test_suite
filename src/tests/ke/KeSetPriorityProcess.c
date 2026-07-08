#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeSetPriorityProcess)
{
    TEST_BEGIN();

    // Get current process via thread's ApcState
    PKPROCESS process = KeGetCurrentThread()->ApcState.Process;
    GEN_CHECK(process != NULL, TRUE, "got current process");

    if (process) {
        // Save current priority
        LONG old_priority = KeSetPriorityProcess(process, 8); // NORMAL
        GEN_CHECK(old_priority > 0, TRUE, "old priority valid");

        // Restore
        KeSetPriorityProcess(process, old_priority);
    }

    TEST_END();
}
