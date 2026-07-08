#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtQueryVirtualMemory)
{
    TEST_BEGIN();

    // Allocate committed memory
    PVOID base = NULL;
    SIZE_T size = 0x10000;
    NTSTATUS status = NtAllocateVirtualMemory(&base, 0, &size,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    GEN_CHECK(status, STATUS_SUCCESS, "alloc");

    if (NT_SUCCESS(status)) {
        MEMORY_BASIC_INFORMATION mbi;

        // --- Query committed region ---
        status = NtQueryVirtualMemory(base, &mbi);
        GEN_CHECK(status, STATUS_SUCCESS, "query committed");
        GEN_CHECK(mbi.BaseAddress, base, "base matches");
        GEN_CHECK(mbi.AllocationBase, base, "alloc base matches");
        GEN_CHECK(mbi.State, MEM_COMMIT, "state=COMMIT");
        GEN_CHECK(mbi.Protect, PAGE_READWRITE, "protect=RW");
        GEN_CHECK(mbi.RegionSize >= 0x10000, TRUE, "size >= requested");
        GEN_CHECK(mbi.Type, MEM_PRIVATE, "type=PRIVATE");

        // --- Query address in the middle of the region ---
        PVOID mid = (PVOID)((ULONG_PTR)base + 0x5000);
        status = NtQueryVirtualMemory(mid, &mbi);
        GEN_CHECK(status, STATUS_SUCCESS, "query middle");
        GEN_CHECK(mbi.AllocationBase, base, "alloc base from middle");

        // --- Decommit a page and query ---
        PVOID decommit_base = base;
        SIZE_T decommit_size = 0x1000;
        NtFreeVirtualMemory(&decommit_base, &decommit_size, MEM_DECOMMIT);

        status = NtQueryVirtualMemory(base, &mbi);
        GEN_CHECK(status, STATUS_SUCCESS, "query decommitted");
        GEN_CHECK(mbi.State, MEM_RESERVE, "decommitted=RESERVE");

        // Free
        PVOID free_base = base;
        SIZE_T free_size = 0;
        NtFreeVirtualMemory(&free_base, &free_size, MEM_RELEASE);
    }

    // --- Query free/unmapped address ---
    MEMORY_BASIC_INFORMATION mbi;
    status = NtQueryVirtualMemory((PVOID)0x50000000, &mbi);
    if (NT_SUCCESS(status)) {
        GEN_CHECK(mbi.State, MEM_FREE, "unmapped=FREE");
    }

    TEST_END();
}
