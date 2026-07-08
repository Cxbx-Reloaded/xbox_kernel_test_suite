#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmAllocateContiguousMemory)
{
    TEST_BEGIN();

    // --- Single page allocation ---
    PVOID mem = MmAllocateContiguousMemory(PAGE_SIZE);
    GEN_CHECK(mem != NULL, TRUE, "1-page allocation");
    if (mem) {
        GEN_CHECK(((ULONG)mem % PAGE_SIZE), 0, "page aligned");
        GEN_CHECK(MmIsAddressValid(mem), TRUE, "address valid");
        memset(mem, 0x55, PAGE_SIZE);
        GEN_CHECK(((PUCHAR)mem)[0], 0x55, "first byte writable");
        GEN_CHECK(((PUCHAR)mem)[PAGE_SIZE - 1], 0x55, "last byte writable");
        PHYSICAL_ADDRESS phys = MmGetPhysicalAddress(mem);
        GEN_CHECK(phys != 0, TRUE, "physical non-zero");
        GEN_CHECK(phys < 0x08000000, TRUE, "within 128MB");
        MmFreeContiguousMemory(mem);
    }

    // --- Multi-page contiguous (verify physical contiguity) ---
    mem = MmAllocateContiguousMemory(PAGE_SIZE * 4);
    GEN_CHECK(mem != NULL, TRUE, "4-page allocation");
    if (mem) {
        PHYSICAL_ADDRESS p0 = MmGetPhysicalAddress(mem);
        PHYSICAL_ADDRESS p1 = MmGetPhysicalAddress((PUCHAR)mem + PAGE_SIZE);
        PHYSICAL_ADDRESS p2 = MmGetPhysicalAddress((PUCHAR)mem + PAGE_SIZE * 2);
        PHYSICAL_ADDRESS p3 = MmGetPhysicalAddress((PUCHAR)mem + PAGE_SIZE * 3);
        GEN_CHECK(p1 - p0, PAGE_SIZE, "pages 0-1 contiguous");
        GEN_CHECK(p2 - p1, PAGE_SIZE, "pages 1-2 contiguous");
        GEN_CHECK(p3 - p2, PAGE_SIZE, "pages 2-3 contiguous");
        memset(mem, 0xAA, PAGE_SIZE * 4);
        GEN_CHECK(((PUCHAR)mem)[PAGE_SIZE * 4 - 1], 0xAA, "last byte of 4 pages");
        MmFreeContiguousMemory(mem);
    }

    // --- Multiple allocations are distinct ---
    PVOID mem1 = MmAllocateContiguousMemory(PAGE_SIZE);
    PVOID mem2 = MmAllocateContiguousMemory(PAGE_SIZE);
    GEN_CHECK(mem1 != NULL, TRUE, "alloc1");
    GEN_CHECK(mem2 != NULL, TRUE, "alloc2");
    if (mem1 && mem2) {
        GEN_CHECK(mem1 != mem2, TRUE, "distinct addresses");
        // Verify no cross-contamination
        memset(mem1, 0x11, PAGE_SIZE);
        memset(mem2, 0x22, PAGE_SIZE);
        GEN_CHECK(((PUCHAR)mem1)[0], 0x11, "mem1 not contaminated");
        GEN_CHECK(((PUCHAR)mem2)[0], 0x22, "mem2 not contaminated");
    }
    if (mem1) MmFreeContiguousMemory(mem1);
    if (mem2) MmFreeContiguousMemory(mem2);

    // --- Large allocation (16 pages = 64KB) ---
    mem = MmAllocateContiguousMemory(PAGE_SIZE * 16);
    GEN_CHECK(mem != NULL, TRUE, "16-page allocation");
    if (mem) {
        GEN_CHECK(((ULONG)mem % PAGE_SIZE), 0, "large alloc page aligned");
        MmFreeContiguousMemory(mem);
    }

    // --- Size 0 ---
    // SKIP: Zero-size contiguous allocation behavior is undefined.
    // May return NULL or a minimum-size block depending on implementation.

    TEST_END();
}
