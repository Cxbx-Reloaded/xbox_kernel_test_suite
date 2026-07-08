#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(FscInvalidateIdleBlocks)
{
    TEST_BEGIN();

    // --- Basic call doesn't crash ---
    FscInvalidateIdleBlocks();
    GEN_CHECK(TRUE, TRUE, "first call returned");

    // --- Multiple calls in succession are safe ---
    FscInvalidateIdleBlocks();
    FscInvalidateIdleBlocks();
    FscInvalidateIdleBlocks();
    GEN_CHECK(TRUE, TRUE, "multiple calls safe");

    // --- Cache size unchanged (invalidation frees content, not capacity) ---
    ULONG before = FscGetCacheSize();
    FscInvalidateIdleBlocks();
    ULONG after = FscGetCacheSize();
    GEN_CHECK(before, after, "cache capacity unchanged");

    // --- Calling after set doesn't crash ---
    ULONG orig = FscGetCacheSize();
    FscSetCacheSize(orig); // ensure set state
    FscInvalidateIdleBlocks();
    GEN_CHECK(FscGetCacheSize(), orig, "still correct after set+invalidate");

    TEST_END();
}
