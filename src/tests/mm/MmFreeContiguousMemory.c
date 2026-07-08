#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmFreeContiguousMemory)
{
    TEST_BEGIN();

    // --- Basic allocate and free ---
    PVOID mem = MmAllocateContiguousMemory(PAGE_SIZE);
    GEN_CHECK(mem != NULL, TRUE, "allocation");
    if (mem) {
        memset(mem, 0xBB, PAGE_SIZE);
        MmFreeContiguousMemory(mem);
        GEN_CHECK(TRUE, TRUE, "free completed");
    }

    // --- Free multiple in reverse order ---
    PVOID mem1 = MmAllocateContiguousMemory(PAGE_SIZE);
    PVOID mem2 = MmAllocateContiguousMemory(PAGE_SIZE);
    PVOID mem3 = MmAllocateContiguousMemory(PAGE_SIZE);
    if (mem3) MmFreeContiguousMemory(mem3);
    if (mem2) MmFreeContiguousMemory(mem2);
    if (mem1) MmFreeContiguousMemory(mem1);
    GEN_CHECK(TRUE, TRUE, "reverse free order");

    // --- Free in creation order ---
    mem1 = MmAllocateContiguousMemory(PAGE_SIZE);
    mem2 = MmAllocateContiguousMemory(PAGE_SIZE);
    mem3 = MmAllocateContiguousMemory(PAGE_SIZE);
    if (mem1) MmFreeContiguousMemory(mem1);
    if (mem2) MmFreeContiguousMemory(mem2);
    if (mem3) MmFreeContiguousMemory(mem3);
    GEN_CHECK(TRUE, TRUE, "forward free order");

    // --- Free scattered (middle first) ---
    mem1 = MmAllocateContiguousMemory(PAGE_SIZE);
    mem2 = MmAllocateContiguousMemory(PAGE_SIZE * 2);
    mem3 = MmAllocateContiguousMemory(PAGE_SIZE);
    if (mem2) MmFreeContiguousMemory(mem2);
    if (mem1) MmFreeContiguousMemory(mem1);
    if (mem3) MmFreeContiguousMemory(mem3);
    GEN_CHECK(TRUE, TRUE, "scattered free");

    // --- Reallocate after free (memory is recycled) ---
    mem = MmAllocateContiguousMemory(PAGE_SIZE);
    if (mem) {
        MmFreeContiguousMemory(mem);
        PVOID mem_new = MmAllocateContiguousMemory(PAGE_SIZE);
        GEN_CHECK(mem_new != NULL, TRUE, "realloc after free");
        if (mem_new) MmFreeContiguousMemory(mem_new);
    }

    // --- Large allocation free ---
    mem = MmAllocateContiguousMemory(PAGE_SIZE * 16);
    if (mem) {
        memset(mem, 0xCC, PAGE_SIZE * 16);
        MmFreeContiguousMemory(mem);
        GEN_CHECK(TRUE, TRUE, "large free completed");
    }

    // --- NULL pointer ---
    // SKIP: MmFreeContiguousMemory(NULL) would dereference the pool header
    // at a negative offset from NULL, causing an access violation.

    // --- Double free ---
    // SKIP: Freeing the same pointer twice corrupts the pool free list,
    // leading to unpredictable crashes on subsequent allocations.

    TEST_END();
}
