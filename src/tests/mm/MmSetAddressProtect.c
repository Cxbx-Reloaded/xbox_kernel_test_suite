#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmSetAddressProtect)
{
    TEST_BEGIN();

    // --- Basic RW -> RO -> RW cycle ---
    PVOID mem = MmAllocateSystemMemory(PAGE_SIZE, PAGE_READWRITE);
    GEN_CHECK(mem != NULL, TRUE, "allocation");
    if (mem) {
        // Write data while RW
        memset(mem, 0xAA, PAGE_SIZE);

        // Change to READONLY
        MmSetAddressProtect(mem, PAGE_SIZE, PAGE_READONLY);
        ULONG protect = MmQueryAddressProtect(mem);
        GEN_CHECK(protect, PAGE_READONLY, "changed to READONLY");

        // Data should be preserved (readable)
        GEN_CHECK(((PUCHAR)mem)[0], 0xAA, "data preserved after RO");
        GEN_CHECK(((PUCHAR)mem)[PAGE_SIZE - 1], 0xAA, "last byte preserved");

        // Change back to READWRITE
        MmSetAddressProtect(mem, PAGE_SIZE, PAGE_READWRITE);
        protect = MmQueryAddressProtect(mem);
        GEN_CHECK(protect, PAGE_READWRITE, "back to READWRITE");

        // Should be writable again
        ((PUCHAR)mem)[0] = 0xBB;
        GEN_CHECK(((PUCHAR)mem)[0], 0xBB, "writable again");

        MmFreeSystemMemory(mem, PAGE_SIZE);
    }

    // --- Multi-page protection change ---
    mem = MmAllocateSystemMemory(PAGE_SIZE * 2, PAGE_READWRITE);
    if (mem) {
        MmSetAddressProtect(mem, PAGE_SIZE * 2, PAGE_READONLY);
        ULONG p1 = MmQueryAddressProtect(mem);
        ULONG p2 = MmQueryAddressProtect((PUCHAR)mem + PAGE_SIZE);
        GEN_CHECK(p1, PAGE_READONLY, "first page READONLY");
        GEN_CHECK(p2, PAGE_READONLY, "second page READONLY");

        MmSetAddressProtect(mem, PAGE_SIZE * 2, PAGE_READWRITE);
        MmFreeSystemMemory(mem, PAGE_SIZE * 2);
    }

    // --- Set PAGE_READWRITE | PAGE_NOCACHE ---
    mem = MmAllocateSystemMemory(PAGE_SIZE, PAGE_READWRITE);
    if (mem) {
        MmSetAddressProtect(mem, PAGE_SIZE, PAGE_READWRITE | PAGE_NOCACHE);
        ULONG protect = MmQueryAddressProtect(mem);
        GEN_CHECK((protect & PAGE_NOCACHE), PAGE_NOCACHE, "nocache flag set");

        // Still writable
        *(volatile ULONG*)mem = 0xDEAD;
        GEN_CHECK(*(volatile ULONG*)mem, 0xDEAD, "nocache writable");

        MmSetAddressProtect(mem, PAGE_SIZE, PAGE_READWRITE);
        MmFreeSystemMemory(mem, PAGE_SIZE);
    }

    // --- Repeated changes don't corrupt ---
    mem = MmAllocateSystemMemory(PAGE_SIZE, PAGE_READWRITE);
    if (mem) {
        *(volatile ULONG*)mem = 0x12345678;
        for (ULONG i = 0; i < 10; i++) {
            MmSetAddressProtect(mem, PAGE_SIZE, PAGE_READONLY);
            MmSetAddressProtect(mem, PAGE_SIZE, PAGE_READWRITE);
        }
        GEN_CHECK(*(volatile ULONG*)mem, 0x12345678, "data survives 10 cycles");
        MmFreeSystemMemory(mem, PAGE_SIZE);
    }

    // --- Writing to READONLY page ---
    // SKIP: Writing to a PAGE_READONLY page would trigger an access violation.
    // The kernel does not provide a safe way to test this without crashing.

    TEST_END();
}
