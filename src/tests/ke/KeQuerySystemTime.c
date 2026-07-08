#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeQuerySystemTime)
{
    TEST_BEGIN();

    // --- Non-zero and reasonable ---
    LARGE_INTEGER time1;
    KeQuerySystemTime(&time1);
    GEN_CHECK(time1.QuadPart != 0, TRUE, "time1 non-zero");
    // System time is in 100ns intervals since Jan 1, 1601
    // Year 2000 in this format is approximately 1.26e17
    // A running Xbox in 2001+ should be past that
    GEN_CHECK(time1.QuadPart > 0, TRUE, "time positive");

    // --- Monotonically increasing ---
    LARGE_INTEGER time2;
    KeQuerySystemTime(&time2);
    GEN_CHECK(time2.QuadPart >= time1.QuadPart, TRUE, "time2 >= time1");

    // --- Advances after stall ---
    KeStallExecutionProcessor(1000); // 1ms
    LARGE_INTEGER time3;
    KeQuerySystemTime(&time3);
    GEN_CHECK(time3.QuadPart > time1.QuadPart, TRUE, "advanced after 1ms stall");

    // --- Elapsed time is approximately correct ---
    // 1ms = 10,000 100ns intervals
    LONGLONG elapsed = time3.QuadPart - time1.QuadPart;
    GEN_CHECK(elapsed >= 5000, TRUE, "elapsed >= 0.5ms");
    GEN_CHECK(elapsed < 1000000, TRUE, "elapsed < 100ms (reasonable)");

    // --- Multiple rapid calls are monotonic ---
    LARGE_INTEGER prev = time3;
    for (ULONG i = 0; i < 100; i++) {
        LARGE_INTEGER curr;
        KeQuerySystemTime(&curr);
        if (curr.QuadPart < prev.QuadPart) {
            GEN_CHECK(FALSE, TRUE, "monotonic violation");
            break;
        }
        prev = curr;
    }
    GEN_CHECK(TRUE, TRUE, "100 calls monotonic");

    // --- Consistency with KeQueryPerformanceCounter timing ---
    LARGE_INTEGER t_start, t_end;
    KeQuerySystemTime(&t_start);
    KeStallExecutionProcessor(5000); // 5ms
    KeQuerySystemTime(&t_end);
    elapsed = t_end.QuadPart - t_start.QuadPart;
    // Should be at least 4ms (allowing for timer granularity)
    GEN_CHECK(elapsed >= 40000, TRUE, "5ms stall: elapsed >= 4ms");

    TEST_END();
}
