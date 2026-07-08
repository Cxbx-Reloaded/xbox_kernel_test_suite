#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(PsQueryStatistics)
{
    TEST_BEGIN();

    PS_STATISTICS stats;
    RtlZeroMemory(&stats, sizeof(stats));
    stats.Length = sizeof(stats);

    NTSTATUS status = PsQueryStatistics(&stats);
    GEN_CHECK(status, STATUS_SUCCESS, "query stats");
    // Should have at least 1 thread (us)
    GEN_CHECK(stats.ThreadCount >= 1, TRUE, "at least 1 thread");

    TEST_END();
}
