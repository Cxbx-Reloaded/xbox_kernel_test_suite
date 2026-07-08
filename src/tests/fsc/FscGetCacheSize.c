#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(FscGetCacheSize)
{
    TEST_BEGIN();

    // --- Cache size should be positive and reasonable ---
    ULONG cache_size = FscGetCacheSize();
    GEN_CHECK(cache_size > 0, TRUE, "cache size > 0");
    // Xbox has 64MB RAM; cache can't exceed total RAM (16384 pages)
    GEN_CHECK(cache_size <= 16384, TRUE, "cache <= total RAM pages");

    // --- Consistency: calling twice gives same result ---
    ULONG cache_size2 = FscGetCacheSize();
    GEN_CHECK(cache_size, cache_size2, "consistent reads");

    // --- After invalidation, size should remain the same ---
    // (invalidation frees contents, not the allocated cache space)
    FscInvalidateIdleBlocks();
    ULONG after_invalidate = FscGetCacheSize();
    GEN_CHECK(after_invalidate, cache_size, "size unchanged after invalidate");

    // --- Cache size is in pages (verify reasonable byte range) ---
    // Minimum reasonable: at least a few pages
    GEN_CHECK(cache_size >= 4, TRUE, "at least 4 pages");

    TEST_END();
}
