#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ExQueryPoolBlockSize)
{
    TEST_BEGIN();

    // --- Small allocation: block size >= requested ---
    PVOID mem = ExAllocatePool(1);
    GEN_CHECK(mem != NULL, TRUE, "1 byte alloc");
    if (mem) {
        ULONG block_size = ExQueryPoolBlockSize(mem);
        GEN_CHECK(block_size >= 1, TRUE, "block_size >= 1");
        // Pool blocks are typically rounded up to 8 or 16 byte boundaries
        GEN_CHECK(block_size <= 64, TRUE, "block_size reasonable for 1 byte");
        ExFreePool(mem);
    }

    // --- Mid-size allocation ---
    mem = ExAllocatePool(48);
    GEN_CHECK(mem != NULL, TRUE, "48 byte alloc");
    if (mem) {
        ULONG block_size = ExQueryPoolBlockSize(mem);
        GEN_CHECK(block_size >= 48, TRUE, "block_size >= 48");
        // Should not be absurdly larger than requested
        GEN_CHECK(block_size <= 128, TRUE, "block_size <= 128 for 48 bytes");
        ExFreePool(mem);
    }

    // --- Page-sized allocation ---
    mem = ExAllocatePool(PAGE_SIZE);
    GEN_CHECK(mem != NULL, TRUE, "PAGE_SIZE alloc");
    if (mem) {
        ULONG block_size = ExQueryPoolBlockSize(mem);
        GEN_CHECK(block_size >= PAGE_SIZE, TRUE, "block_size >= PAGE_SIZE");
        ExFreePool(mem);
    }

    // --- Multi-page allocation ---
    mem = ExAllocatePool(PAGE_SIZE * 3);
    GEN_CHECK(mem != NULL, TRUE, "3 page alloc");
    if (mem) {
        ULONG block_size = ExQueryPoolBlockSize(mem);
        GEN_CHECK(block_size >= PAGE_SIZE * 3, TRUE, "block_size >= 3 pages");
        ExFreePool(mem);
    }

    // --- Tagged allocation should also be queryable ---
    mem = ExAllocatePoolWithTag(100, 'tseT');
    GEN_CHECK(mem != NULL, TRUE, "tagged alloc");
    if (mem) {
        ULONG block_size = ExQueryPoolBlockSize(mem);
        GEN_CHECK(block_size >= 100, TRUE, "tagged block_size >= 100");
        ExFreePool(mem);
    }

    // --- NULL pointer ---
    // SKIP: ExQueryPoolBlockSize(NULL) is undefined behavior. The function
    // dereferences the pool header preceding the pointer, so NULL would read
    // from address (NULL - header_size) which is unmapped. This would crash.

    // --- Freed pointer ---
    // SKIP: Querying a freed block is undefined - the pool header may have been
    // overwritten by the free-list management. Results would be garbage or crash.

    TEST_END();
}
