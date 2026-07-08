#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ExSaveNonVolatileSetting)
{
    TEST_BEGIN();

    // Read current timezone bias, write it back (no-op save)
    ULONG type = 0;
    ULONG value = 0;
    ULONG result_length = 0;

    NTSTATUS status = ExQueryNonVolatileSetting(
        XC_TIMEZONE_BIAS,
        &type,
        &value,
        sizeof(value),
        &result_length
    );

    if (NT_SUCCESS(status)) {
        // Save the same value back (safe - doesn't change anything)
        status = ExSaveNonVolatileSetting(
            XC_TIMEZONE_BIAS,
            REG_DWORD,
            &value,
            sizeof(value)
        );
        GEN_CHECK(status, STATUS_SUCCESS, "save same value back");
    }

    TEST_END();
}
