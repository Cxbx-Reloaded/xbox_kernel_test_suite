#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ExAllocatePool)
{
    TEST_BEGIN();

    // --- Basic allocation and write verification ---
    PVOID mem = ExAllocatePool(32);
    GEN_CHECK(mem != NULL, TRUE, "32 byte allocation");
    if (mem) {
        // Fill entire block, verify read-back
        memset(mem, 0xAA, 32);
        GEN_CHECK(((PUCHAR)mem)[0], 0xAA, "first byte written");
        GEN_CHECK(((PUCHAR)mem)[31], 0xAA, "last byte written");
        ExFreePool(mem);
    }

    // --- Page-sized allocation ---
    mem = ExAllocatePool(PAGE_SIZE);
    GEN_CHECK(mem != NULL, TRUE, "PAGE_SIZE allocation");
    if (mem) {
        BOOLEAN is_valid = MmIsAddressValid(mem);
        GEN_CHECK(is_valid, TRUE, "valid address");
        // Write to first and last byte of page
        ((PUCHAR)mem)[0] = 0x55;
        ((PUCHAR)mem)[PAGE_SIZE - 1] = 0xAA;
        GEN_CHECK(((PUCHAR)mem)[0], 0x55, "page first byte");
        GEN_CHECK(((PUCHAR)mem)[PAGE_SIZE - 1], 0xAA, "page last byte");
        ExFreePool(mem);
    }

    // --- Minimum allocation (1 byte) ---
    mem = ExAllocatePool(1);
    GEN_CHECK(mem != NULL, TRUE, "1 byte allocation");
    if (mem) {
        *(PUCHAR)mem = 0xFF;
        GEN_CHECK(*(PUCHAR)mem, 0xFF, "1 byte writable");
        ExFreePool(mem);
    }

    // --- Multiple allocations return distinct addresses ---
    PVOID mem1 = ExAllocatePool(64);
    PVOID mem2 = ExAllocatePool(64);
    GEN_CHECK(mem1 != NULL, TRUE, "multi alloc 1");
    GEN_CHECK(mem2 != NULL, TRUE, "multi alloc 2");
    if (mem1 && mem2) {
        GEN_CHECK(mem1 != mem2, TRUE, "distinct addresses");
        // Writing to one shouldn't affect the other
        memset(mem1, 0x11, 64);
        memset(mem2, 0x22, 64);
        GEN_CHECK(((PUCHAR)mem1)[0], 0x11, "no cross-contamination mem1");
        GEN_CHECK(((PUCHAR)mem2)[0], 0x22, "no cross-contamination mem2");
    }
    if (mem2) ExFreePool(mem2);
    if (mem1) ExFreePool(mem1);

    // --- Large allocation (multi-page) ---
    mem = ExAllocatePool(PAGE_SIZE * 4);
    GEN_CHECK(mem != NULL, TRUE, "4-page allocation");
    if (mem) {
        // Write across page boundaries
        ((PUCHAR)mem)[PAGE_SIZE - 1] = 0xBB;
        ((PUCHAR)mem)[PAGE_SIZE] = 0xCC;
        GEN_CHECK(((PUCHAR)mem)[PAGE_SIZE - 1], 0xBB, "cross-page boundary -1");
        GEN_CHECK(((PUCHAR)mem)[PAGE_SIZE], 0xCC, "cross-page boundary +0");
        ExFreePool(mem);
    }

    // --- Allocation of 0 bytes ---
    // SKIP: Allocating 0 bytes is undefined behavior on Xbox kernel.
    // On Windows NT, ExAllocatePool(0) may return a valid pointer to a
    // minimum-sized block or crash. Testing this risks system instability.

    TEST_END();
}
