#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(FscSetCacheSize)
{
    TEST_BEGIN();

    // --- Save original cache size ---
    ULONG original_size = FscGetCacheSize();
    GEN_CHECK(original_size > 0, TRUE, "original > 0");

    // --- Set to same value (no-op) ---
    FscSetCacheSize(original_size);
    ULONG result = FscGetCacheSize();
    GEN_CHECK(result, original_size, "set same value");

    // --- Increase cache size slightly ---
    ULONG larger = original_size + 4;
    FscSetCacheSize(larger);
    result = FscGetCacheSize();
    GEN_CHECK(result, larger, "increased size");

    // --- Decrease back to original ---
    FscSetCacheSize(original_size);
    result = FscGetCacheSize();
    GEN_CHECK(result, original_size, "restored original");

    // --- Set to smaller value ---
    if (original_size > 8) {
        ULONG smaller = original_size - 4;
        FscSetCacheSize(smaller);
        result = FscGetCacheSize();
        GEN_CHECK(result, smaller, "decreased size");
        // Restore
        FscSetCacheSize(original_size);
    }

    // --- Multiple set calls in sequence ---
    FscSetCacheSize(original_size + 1);
    FscSetCacheSize(original_size + 2);
    FscSetCacheSize(original_size + 3);
    result = FscGetCacheSize();
    GEN_CHECK(result, original_size + 3, "last set wins");

    // Restore
    FscSetCacheSize(original_size);
    GEN_CHECK(FscGetCacheSize(), original_size, "final restore");

    // --- Size 0 ---
    // SKIP: Setting cache size to 0 may disable filesystem caching entirely,
    // causing I/O failures or hangs. Not safe to test.

    TEST_END();
}
