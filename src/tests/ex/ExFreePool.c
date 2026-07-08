#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ExFreePool)
{
    TEST_BEGIN();

    // --- Basic alloc/free cycle ---
    PVOID mem = ExAllocatePool(64);
    GEN_CHECK(mem != NULL, TRUE, "basic alloc");
    if (mem) {
        memset(mem, 0xDD, 64);
        ExFreePool(mem);
        GEN_CHECK(TRUE, TRUE, "basic free completed");
    }

    // --- Free multiple blocks in different order ---
    // Tests that the pool allocator handles non-LIFO free patterns
    PVOID mem1 = ExAllocatePool(32);
    PVOID mem2 = ExAllocatePool(64);
    PVOID mem3 = ExAllocatePool(128);
    PVOID mem4 = ExAllocatePool(256);

    GEN_CHECK(mem1 != NULL, TRUE, "alloc1");
    GEN_CHECK(mem2 != NULL, TRUE, "alloc2");
    GEN_CHECK(mem3 != NULL, TRUE, "alloc3");
    GEN_CHECK(mem4 != NULL, TRUE, "alloc4");

    // Free in scattered order: 3, 1, 4, 2
    if (mem3) ExFreePool(mem3);
    if (mem1) ExFreePool(mem1);
    if (mem4) ExFreePool(mem4);
    if (mem2) ExFreePool(mem2);

    GEN_CHECK(TRUE, TRUE, "scattered free order completed");

    // --- Verify freed memory can be reallocated ---
    // After freeing, a new allocation should succeed (pool isn't exhausted)
    mem = ExAllocatePool(128);
    GEN_CHECK(mem != NULL, TRUE, "realloc after free");
    if (mem) {
        // Verify the memory is usable (not corrupted free-list metadata)
        memset(mem, 0x00, 128);
        GEN_CHECK(((PUCHAR)mem)[0], 0x00, "reallocated memory clean");
        GEN_CHECK(((PUCHAR)mem)[127], 0x00, "reallocated memory clean end");
        ExFreePool(mem);
    }

    // --- Free of NULL ---
    // SKIP: ExFreePool(NULL) is undefined behavior and will likely bugcheck.
    // The kernel pool allocator does not validate the pointer - passing NULL
    // or a previously-freed pointer causes a system crash. No title should
    // ever call ExFreePool with an invalid pointer.

    // --- Double free ---
    // SKIP: Double-freeing pool memory corrupts the pool allocator's internal
    // data structures, leading to a delayed bugcheck or immediate crash.
    // This is undefined behavior that no correctly-written software depends on.

    TEST_END();
}
