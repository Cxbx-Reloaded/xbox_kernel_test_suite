#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeQueryPerformanceFrequency)
{
    TEST_BEGIN();

    // --- Non-zero ---
    ULONGLONG freq = KeQueryPerformanceFrequency();
    GEN_CHECK(freq != 0, TRUE, "frequency non-zero");

    // --- Xbox ACPI PM timer: 3,579,545 Hz ---
    // Allow range for different timer sources
    GEN_CHECK(freq > 1000000, TRUE, "frequency > 1MHz");
    GEN_CHECK(freq < 4000000, TRUE, "frequency < 4MHz");

    // --- Exact value check (ACPI timer) ---
    // The ACPI PM timer runs at exactly 3,579,545 Hz (1/3 of NTSC colorburst)
    GEN_CHECK(freq, (ULONGLONG)3579545, "ACPI PM timer frequency");

    // --- Consistent across calls ---
    ULONGLONG freq2 = KeQueryPerformanceFrequency();
    GEN_CHECK(freq2, freq, "consistent call 2");

    ULONGLONG freq3 = KeQueryPerformanceFrequency();
    GEN_CHECK(freq3, freq, "consistent call 3");

    // --- Fits in 32 bits ---
    GEN_CHECK(freq <= 0xFFFFFFFF, TRUE, "fits in 32 bits");

    // --- Useful for timing calculations ---
    // 1 second = freq ticks
    // 1ms = freq / 1000 ticks
    ULONGLONG ticks_per_ms = freq / 1000;
    GEN_CHECK(ticks_per_ms > 1000, TRUE, "ticks/ms > 1000");
    GEN_CHECK(ticks_per_ms < 4000, TRUE, "ticks/ms < 4000");

    TEST_END();
}
