#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeStallExecutionProcessor)
{
    TEST_BEGIN();

    // Stall for a known duration and verify time has elapsed using perf counter
    ULONGLONG freq = KeQueryPerformanceFrequency();
    ULONGLONG before = KeQueryPerformanceCounter();

    // Stall for 10,000 microseconds (10ms)
    KeStallExecutionProcessor(10000);

    ULONGLONG after = KeQueryPerformanceCounter();
    ULONGLONG elapsed_ticks = after - before;

    // Convert to microseconds: elapsed_us = elapsed_ticks * 1000000 / freq
    // We expect approximately 10000us, allow 5000-50000us tolerance
    ULONGLONG elapsed_us = (elapsed_ticks * 1000000) / freq;
    GEN_CHECK(elapsed_us >= 5000, TRUE, "elapsed >= 5ms");
    GEN_CHECK(elapsed_us <= 50000, TRUE, "elapsed <= 50ms");

    // Stalling 0 should not crash
    KeStallExecutionProcessor(0);

    TEST_END();
}
