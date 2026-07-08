#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeQueryInterruptTime)
{
    TEST_BEGIN();

    ULONGLONG time1 = KeQueryInterruptTime();
    GEN_CHECK(time1 != 0, TRUE, "interrupt time non-zero");

    // Should be monotonically increasing
    ULONGLONG time2 = KeQueryInterruptTime();
    GEN_CHECK(time2 >= time1, TRUE, "time2 >= time1");

    // After stall, should advance
    KeStallExecutionProcessor(2000); // 2ms
    ULONGLONG time3 = KeQueryInterruptTime();
    GEN_CHECK(time3 > time1, TRUE, "time3 > time1 after stall");

    TEST_END();
}
