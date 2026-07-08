#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmQueryAllocationSize)
{
    TEST_BEGIN();

    // --- Single page ---
    PVOID mem = MmAllocateSystemMemory(PAGE_SIZE, PAGE_READWRITE);
    GEN_CHECK(mem != NULL, TRUE, "1-page alloc");
    if (mem) {
        ULONG size = MmQueryAllocationSize(mem);
        GEN_CHECK(size >= PAGE_SIZE, TRUE, "size >= PAGE_SIZE");
        // Should not be unreasonably large
        GEN_CHECK(size <= PAGE_SIZE * 2, TRUE, "size <= 2 pages (overhead)");
        MmFreeSystemMemory(mem, PAGE_SIZE);
    }

    // --- Multiple pages ---
    mem = MmAllocateSystemMemory(PAGE_SIZE * 4, PAGE_READWRITE);
    if (mem) {
        ULONG size = MmQueryAllocationSize(mem);
        GEN_CHECK(size >= PAGE_SIZE * 4, TRUE, "size >= 4 pages");
        GEN_CHECK(size <= PAGE_SIZE * 5, TRUE, "size <= 5 pages");
        MmFreeSystemMemory(mem, PAGE_SIZE * 4);
    }

    // --- Contiguous memory ---
    mem = MmAllocateContiguousMemory(PAGE_SIZE * 2);
    if (mem) {
        ULONG size = MmQueryAllocationSize(mem);
        GEN_CHECK(size >= PAGE_SIZE * 2, TRUE, "contiguous >= 2 pages");
        MmFreeContiguousMemory(mem);
    }

    // --- Pool allocations ---
    // NOTE: MmQueryAllocationSize is designed for MmAllocateContiguousMemory
    // and MmAllocateSystemMemory only. For pool blocks (ExAllocatePool), it
    // walks the PTEs of the underlying page allocation and returns PAGE_SIZE
    // (since pool pages are single-page MmAllocateSystemMemory allocations).
    mem = ExAllocatePool(64);
    if (mem) {
        ULONG size = MmQueryAllocationSize(mem);
        GEN_CHECK(size, PAGE_SIZE, "pool block returns PAGE_SIZE");
        ExFreePool(mem);
    }

    // --- Larger pool allocation (still sub-page) ---
    mem = ExAllocatePool(1024);
    if (mem) {
        ULONG size = MmQueryAllocationSize(mem);
        GEN_CHECK(size, PAGE_SIZE, "1024-byte pool returns PAGE_SIZE");
        ExFreePool(mem);
    }

    // --- Different sizes return proportionally different results ---
    PVOID small = MmAllocateSystemMemory(PAGE_SIZE, PAGE_READWRITE);
    PVOID large = MmAllocateSystemMemory(PAGE_SIZE * 8, PAGE_READWRITE);
    if (small && large) {
        ULONG small_size = MmQueryAllocationSize(small);
        ULONG large_size = MmQueryAllocationSize(large);
        GEN_CHECK(large_size > small_size, TRUE, "larger alloc = larger size");
    }
    if (small) MmFreeSystemMemory(small, PAGE_SIZE);
    if (large) MmFreeSystemMemory(large, PAGE_SIZE * 8);

    // --- NULL/freed pointer ---
    // SKIP: MmQueryAllocationSize on NULL or freed memory reads from the
    // allocation header at a negative offset, causing undefined behavior.

    TEST_END();
}
