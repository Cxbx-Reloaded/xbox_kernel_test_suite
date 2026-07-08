#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmQueryStatistics)
{
    TEST_BEGIN();

    // --- Part 1: Verify all invalid Length values are rejected ---
    // sizeof(MM_STATISTICS) == 0x24. Only exactly 0x24 is accepted.
    {
        const unsigned valid_length = sizeof(MM_STATISTICS); // 0x24

        // Test all too-small lengths (0x00 to 0x23)
        for (unsigned j = 0; j < valid_length; ++j) {
            MM_STATISTICS mm;
            memset(&mm, 0, sizeof(mm));
            mm.Length = j;

            // Save a copy to verify struct wasn't written to
            MM_STATISTICS mm_before;
            memcpy(&mm_before, &mm, sizeof(mm));

            NTSTATUS ret = MmQueryStatistics(&mm);
            GEN_CHECK(ret, STATUS_INVALID_PARAMETER, "invalid Length rejected");

            // Verify struct was not modified (beyond Length field already set)
            GEN_CHECK(memcmp(&mm, &mm_before, sizeof(mm)) == 0, TRUE,
                      "struct not written on invalid Length");
        }

        // Length > sizeof must also fail
        {
            MM_STATISTICS mm;
            memset(&mm, 0, sizeof(mm));
            mm.Length = valid_length + 1;
            NTSTATUS ret = MmQueryStatistics(&mm);
            GEN_CHECK(ret, STATUS_INVALID_PARAMETER, "Length > sizeof rejected");
        }
    }

    // --- Part 2: Valid query with correct Length ---
    MM_STATISTICS stats;
    memset(&stats, 0, sizeof(stats));
    stats.Length = sizeof(stats);

    NTSTATUS status = MmQueryStatistics(&stats);
    GEN_CHECK(status, STATUS_SUCCESS, "query succeeds with sizeof");

    // Xbox has 64MB (16384 pages) or 128MB (32768 pages)
    GEN_CHECK(stats.TotalPhysicalPages > 0, TRUE, "TotalPhysicalPages > 0");
    GEN_CHECK(stats.TotalPhysicalPages >= 16384, TRUE, "at least 64MB");
    GEN_CHECK(stats.TotalPhysicalPages <= 32768, TRUE, "at most 128MB");

    GEN_CHECK(stats.AvailablePages > 0, TRUE, "AvailablePages > 0");
    GEN_CHECK(stats.AvailablePages <= stats.TotalPhysicalPages, TRUE, "Available <= Total");

    GEN_CHECK(stats.VirtualMemoryBytesCommitted > 0, TRUE, "committed > 0");

    // --- Part 3: Consistency check ---
    MM_STATISTICS stats2;
    memset(&stats2, 0, sizeof(stats2));
    stats2.Length = sizeof(stats2);
    MmQueryStatistics(&stats2);
    GEN_CHECK(stats2.TotalPhysicalPages, stats.TotalPhysicalPages,
              "total pages consistent across calls");

    TEST_END();
}
