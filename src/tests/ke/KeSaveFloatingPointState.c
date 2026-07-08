#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeSaveFloatingPointState)
{
    TEST_BEGIN();

    KFLOATING_SAVE save;
    NTSTATUS status = KeSaveFloatingPointState(&save);
    GEN_CHECK(status, STATUS_SUCCESS, "save FP state");

    // Restore it
    if (NT_SUCCESS(status)) {
        KeRestoreFloatingPointState(&save);
    }

    TEST_END();
}
