#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeSystemTime)
{
    TEST_BEGIN();

    // KeSystemTime is an exported variable (KSYSTEM_TIME structure)
    // It contains the system time in 100ns units since Jan 1, 1601
    LARGE_INTEGER time1;
    time1.LowPart = KeSystemTime.LowPart;
    time1.HighPart = KeSystemTime.High1Time;
    GEN_CHECK(time1.QuadPart != 0, TRUE, "system time non-zero");

    // High1Time and High2Time should match (consistency check)
    GEN_CHECK(KeSystemTime.High1Time, KeSystemTime.High2Time, "High1Time == High2Time");

    // After a delay, it should advance
    KeStallExecutionProcessor(5000); // 5ms
    LARGE_INTEGER time2;
    time2.LowPart = KeSystemTime.LowPart;
    time2.HighPart = KeSystemTime.High1Time;
    GEN_CHECK(time2.QuadPart >= time1.QuadPart, TRUE, "time increases");

    TEST_END();
}
