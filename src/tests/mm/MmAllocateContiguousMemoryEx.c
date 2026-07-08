#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmAllocateContiguousMemoryEx)
{
    TEST_BEGIN();

    // --- Basic allocation with full address range ---
    PVOID mem = MmAllocateContiguousMemoryEx(
        PAGE_SIZE,       // NumberOfBytes
        0,               // LowestAcceptableAddress
        0x03FFFFFF,      // HighestAcceptableAddress (64MB)
        0,               // Alignment (0 = page aligned)
        PAGE_READWRITE   // Protect
    );
    GEN_CHECK(mem != NULL, TRUE, "basic allocation");
    if (mem) {
        GEN_CHECK(((ULONG)mem % PAGE_SIZE), 0, "page aligned");
        PHYSICAL_ADDRESS phys = MmGetPhysicalAddress(mem);
        GEN_CHECK(phys <= 0x03FFFFFF, TRUE, "within address range");
        // Verify read/write
        memset(mem, 0x55, PAGE_SIZE);
        GEN_CHECK(((PUCHAR)mem)[0], 0x55, "first byte writable");
        GEN_CHECK(((PUCHAR)mem)[PAGE_SIZE - 1], 0x55, "last byte writable");
        MmFreeContiguousMemory(mem);
    }

    // --- 64KB alignment ---
    mem = MmAllocateContiguousMemoryEx(
        PAGE_SIZE, 0, 0x7FFFFFFF,
        0x10000,         // 64KB alignment
        PAGE_READWRITE
    );
    GEN_CHECK(mem != NULL, TRUE, "64KB aligned allocation");
    if (mem) {
        GEN_CHECK(((ULONG)mem % 0x10000), 0, "64KB aligned");
        MmFreeContiguousMemory(mem);
    }

    // --- Multi-page contiguous allocation ---
    mem = MmAllocateContiguousMemoryEx(
        PAGE_SIZE * 4, 0, 0x7FFFFFFF, 0, PAGE_READWRITE
    );
    GEN_CHECK(mem != NULL, TRUE, "4-page contiguous");
    if (mem) {
        // Verify physical contiguity
        PHYSICAL_ADDRESS phys0 = MmGetPhysicalAddress(mem);
        PHYSICAL_ADDRESS phys1 = MmGetPhysicalAddress((PUCHAR)mem + PAGE_SIZE);
        PHYSICAL_ADDRESS phys2 = MmGetPhysicalAddress((PUCHAR)mem + PAGE_SIZE * 2);
        PHYSICAL_ADDRESS phys3 = MmGetPhysicalAddress((PUCHAR)mem + PAGE_SIZE * 3);
        GEN_CHECK(phys1 - phys0, PAGE_SIZE, "pages 0-1 contiguous");
        GEN_CHECK(phys2 - phys1, PAGE_SIZE, "pages 1-2 contiguous");
        GEN_CHECK(phys3 - phys2, PAGE_SIZE, "pages 2-3 contiguous");
        MmFreeContiguousMemory(mem);
    }

    // --- PAGE_READONLY protection ---
    mem = MmAllocateContiguousMemoryEx(
        PAGE_SIZE, 0, 0x7FFFFFFF, 0, PAGE_READONLY
    );
    GEN_CHECK(mem != NULL, TRUE, "readonly allocation");
    if (mem) {
        ULONG protect = MmQueryAddressProtect(mem);
        GEN_CHECK(protect, PAGE_READONLY, "protection is READONLY");
        MmFreeContiguousMemory(mem);
    }

    // --- PAGE_READWRITE | PAGE_NOCACHE (common for DMA buffers) ---
    mem = MmAllocateContiguousMemoryEx(
        PAGE_SIZE, 0, 0x7FFFFFFF, 0, PAGE_READWRITE | PAGE_NOCACHE
    );
    GEN_CHECK(mem != NULL, TRUE, "nocache allocation");
    if (mem) {
        *(volatile ULONG*)mem = 0xDEADC0DE;
        GEN_CHECK(*(volatile ULONG*)mem, 0xDEADC0DE, "nocache writable");
        MmFreeContiguousMemory(mem);
    }

    // --- PAGE_READWRITE | PAGE_WRITECOMBINE (for GPU buffers) ---
    mem = MmAllocateContiguousMemoryEx(
        PAGE_SIZE, 0, 0x7FFFFFFF, 0, PAGE_READWRITE | PAGE_WRITECOMBINE
    );
    GEN_CHECK(mem != NULL, TRUE, "writecombine allocation");
    if (mem) {
        *(volatile ULONG*)mem = 0xFACEFEED;
        GEN_CHECK(*(volatile ULONG*)mem, 0xFACEFEED, "writecombine writable");
        MmFreeContiguousMemory(mem);
    }

    // --- Conflicting cache attributes should fail ---
    mem = MmAllocateContiguousMemoryEx(
        PAGE_SIZE, 0, 0x7FFFFFFF, 0,
        PAGE_READWRITE | PAGE_NOCACHE | PAGE_WRITECOMBINE
    );
    // NOCACHE and WRITECOMBINE are mutually exclusive
    GEN_CHECK(mem == NULL, TRUE, "conflicting cache attrs returns NULL");
    if (mem) {
        MmFreeContiguousMemory(mem);
    }

    // --- LowestAcceptable > HighestAcceptable (impossible range) ---
    mem = MmAllocateContiguousMemoryEx(
        PAGE_SIZE,
        0x04000000,  // Lowest = 64MB
        0x01000000,  // Highest = 16MB (impossible)
        0, PAGE_READWRITE
    );
    GEN_CHECK(mem == NULL, TRUE, "impossible address range returns NULL");
    if (mem) {
        MmFreeContiguousMemory(mem);
    }

    // --- Very large alignment (must be power of 2) ---
    mem = MmAllocateContiguousMemoryEx(
        PAGE_SIZE, 0, 0x7FFFFFFF,
        0x100000,    // 1MB alignment
        PAGE_READWRITE
    );
    if (mem) {
        GEN_CHECK(((ULONG)mem % 0x100000), 0, "1MB aligned");
        MmFreeContiguousMemory(mem);
    }
    // Note: May fail if no 1MB-aligned contiguous block available. That's OK.

    // --- Size 0 ---
    // SKIP: Allocating 0 bytes of contiguous memory is undefined behavior.
    // The kernel may return NULL, return a minimum-size block, or crash.
    // No legitimate software requests 0-byte contiguous allocations.

    TEST_END();
}
