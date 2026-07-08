#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeQueryPerformanceCounter)
{
    TEST_BEGIN();

    // --- Non-zero ---
    ULONGLONG counter1 = KeQueryPerformanceCounter();
    GEN_CHECK(counter1 != 0, TRUE, "counter non-zero");

    // --- Monotonically increasing ---
    ULONGLONG counter2 = KeQueryPerformanceCounter();
    GEN_CHECK(counter2 >= counter1, TRUE, "monotonic");

    // --- Advances after stall ---
    KeStallExecutionProcessor(100); // 100us
    ULONGLONG counter3 = KeQueryPerformanceCounter();
    GEN_CHECK(counter3 > counter1, TRUE, "advanced after stall");

    // --- Elapsed ticks roughly match expected ---
    // At ~3.579MHz, 100us = ~358 ticks
    ULONGLONG ticks = counter3 - counter1;
    GEN_CHECK(ticks > 100, TRUE, "at least 100 ticks for 100us");
    GEN_CHECK(ticks < 100000, TRUE, "less than 100K ticks (reasonable)");

    // --- Consistent with KeQueryPerformanceFrequency ---
    ULONGLONG freq = KeQueryPerformanceFrequency();
    ULONGLONG start = KeQueryPerformanceCounter();
    KeStallExecutionProcessor(10000); // 10ms
    ULONGLONG end = KeQueryPerformanceCounter();
    ULONGLONG elapsed_ticks = end - start;
    // Expected: freq * 0.01 = ticks for 10ms
    ULONGLONG expected = freq / 100;
    // Allow 50% tolerance for timer inaccuracy
    GEN_CHECK(elapsed_ticks > expected / 2, TRUE, "10ms: ticks > half expected");
    GEN_CHECK(elapsed_ticks < expected * 3, TRUE, "10ms: ticks < 3x expected");

    // --- Multiple rapid calls are strictly non-decreasing ---
    ULONGLONG prev = KeQueryPerformanceCounter();
    for (ULONG i = 0; i < 100; i++) {
        ULONGLONG curr = KeQueryPerformanceCounter();
        if (curr < prev) {
            GEN_CHECK(FALSE, TRUE, "monotonic violation");
            break;
        }
        prev = curr;
    }
    GEN_CHECK(TRUE, TRUE, "100 calls monotonic");

    TEST_END();
}
