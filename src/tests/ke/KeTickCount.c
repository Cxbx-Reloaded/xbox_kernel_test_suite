#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeTickCount)
{
    TEST_BEGIN();

    // KeTickCount is a kernel-exported variable (pointer to tick count)
    // It should be non-zero after boot and monotonically increasing
    volatile ULONG tick1 = KeTickCount;
    GEN_CHECK(tick1 != 0, TRUE, "tick count non-zero");

    // Stall for a bit and check it increases
    KeStallExecutionProcessor(20000); // 20ms, should be at least 1 tick (1 tick = ~1ms)
    volatile ULONG tick2 = KeTickCount;
    GEN_CHECK(tick2 >= tick1, TRUE, "tick2 >= tick1");

    TEST_END();
}
