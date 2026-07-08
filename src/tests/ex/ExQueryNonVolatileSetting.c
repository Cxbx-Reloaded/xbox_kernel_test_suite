#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ExQueryNonVolatileSetting)
{
    TEST_BEGIN();

    // Query timezone bias (EEPROM setting)
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
    GEN_CHECK(status, STATUS_SUCCESS, "query timezone bias");
    GEN_CHECK(type, REG_DWORD, "type is DWORD");
    GEN_CHECK(result_length, sizeof(ULONG), "result length");

    // Query language
    ULONG lang = 0;
    result_length = 0;
    status = ExQueryNonVolatileSetting(
        XC_LANGUAGE,
        &type,
        &lang,
        sizeof(lang),
        &result_length
    );
    GEN_CHECK(status, STATUS_SUCCESS, "query language");

    TEST_END();
}
