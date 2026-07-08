#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ExInterlockedAddLargeStatistic)
{
    TEST_BEGIN();

    LARGE_INTEGER addend;

    // --- Basic add, no carry ---
    addend.QuadPart = 1000;
    ExInterlockedAddLargeStatistic(&addend, 500);
    GEN_CHECK(addend.QuadPart, 1500, "basic add 500");

    // --- Add zero (no change) ---
    addend.QuadPart = 42;
    ExInterlockedAddLargeStatistic(&addend, 0);
    GEN_CHECK(addend.QuadPart, 42, "add zero no change");

    // --- Add to zero ---
    addend.QuadPart = 0;
    ExInterlockedAddLargeStatistic(&addend, 100);
    GEN_CHECK(addend.QuadPart, 100, "add to zero");

    // --- Carry from low to high (boundary) ---
    addend.QuadPart = 0x00000000FFFFFFFE;
    ExInterlockedAddLargeStatistic(&addend, 3);
    GEN_CHECK(addend.QuadPart, 0x0000000100000001, "carry to high word");

    // --- Carry from 0xFFFFFFFF to 0x100000000 ---
    addend.QuadPart = 0x00000000FFFFFFFF;
    ExInterlockedAddLargeStatistic(&addend, 1);
    GEN_CHECK(addend.QuadPart, 0x0000000100000000, "exact boundary carry");

    // --- Multiple carries accumulate in high word ---
    addend.QuadPart = 0x00000005FFFFFFF0;
    ExInterlockedAddLargeStatistic(&addend, 0x20);
    GEN_CHECK(addend.QuadPart, 0x0000000600000010, "carry increments high");

    // --- Near-max low word without carry ---
    addend.QuadPart = 0x00000000FFFFFFF0;
    ExInterlockedAddLargeStatistic(&addend, 5);
    GEN_CHECK(addend.QuadPart, 0x00000000FFFFFFF5, "near max no carry");

    // --- Large increment (max ULONG value) ---
    addend.QuadPart = 0;
    ExInterlockedAddLargeStatistic(&addend, 0xFFFFFFFF);
    GEN_CHECK(addend.QuadPart, 0x00000000FFFFFFFF, "add max ULONG to 0");

    // --- Large increment that causes carry ---
    addend.QuadPart = 1;
    ExInterlockedAddLargeStatistic(&addend, 0xFFFFFFFF);
    GEN_CHECK(addend.QuadPart, 0x0000000100000000, "max ULONG + 1 carry");

    // --- High word already large ---
    addend.QuadPart = 0x7FFFFFFF00000000;
    ExInterlockedAddLargeStatistic(&addend, 100);
    GEN_CHECK(addend.QuadPart, 0x7FFFFFFF00000064, "high word already large");

    // --- Accumulate many small adds ---
    addend.QuadPart = 0;
    for (ULONG i = 0; i < 1000; i++) {
        ExInterlockedAddLargeStatistic(&addend, 1);
    }
    GEN_CHECK(addend.QuadPart, 1000, "1000 increments of 1");

    // Note: ExInterlockedAddLargeStatistic only accepts ULONG increment
    // (unsigned 32-bit), so negative values cannot be tested. The function
    // does NOT use a spinlock - it relies on lock-prefix instructions.

    TEST_END();
}
