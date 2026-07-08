#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeInterruptTime)
{
    TEST_BEGIN();

    // KeInterruptTime is an exported variable (KSYSTEM_TIME structure)
    // It contains the interrupt time in 100ns units
    LARGE_INTEGER time1;
    time1.LowPart = KeInterruptTime.LowPart;
    time1.HighPart = KeInterruptTime.High1Time;
    GEN_CHECK(time1.QuadPart != 0, TRUE, "interrupt time non-zero");

    // After a delay, it should advance
    KeStallExecutionProcessor(5000); // 5ms
    LARGE_INTEGER time2;
    time2.LowPart = KeInterruptTime.LowPart;
    time2.HighPart = KeInterruptTime.High1Time;
    GEN_CHECK(time2.QuadPart >= time1.QuadPart, TRUE, "time increases");

    TEST_END();
}
