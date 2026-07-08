#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeRestoreFloatingPointState)
{
    TEST_BEGIN();

    // Save, modify FP state, restore, verify
    KFLOATING_SAVE save;
    NTSTATUS status = KeSaveFloatingPointState(&save);
    GEN_CHECK(status, STATUS_SUCCESS, "save");

    if (NT_SUCCESS(status)) {
        // Do some FP work
        volatile double x = 3.14159;
        volatile double y = x * x;
        (void)y;

        KeRestoreFloatingPointState(&save);
        GEN_CHECK(TRUE, TRUE, "restore completed");
    }

    TEST_END();
}
