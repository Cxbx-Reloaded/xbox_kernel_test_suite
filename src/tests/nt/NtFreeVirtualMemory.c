#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtFreeVirtualMemory)
{
    TEST_BEGIN();

    // --- MEM_RELEASE: release entire allocation ---
    PVOID base = NULL;
    SIZE_T size = 0x10000;
    NTSTATUS status = NtAllocateVirtualMemory(&base, 0, &size,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    GEN_CHECK(status, STATUS_SUCCESS, "alloc for release");

    if (NT_SUCCESS(status)) {
        PVOID free_base = base;
        SIZE_T free_size = 0; // must be 0 for MEM_RELEASE
        status = NtFreeVirtualMemory(&free_base, &free_size, MEM_RELEASE);
        GEN_CHECK(status, STATUS_SUCCESS, "MEM_RELEASE");
    }

    // --- MEM_DECOMMIT: decommit pages but keep reservation ---
    base = NULL;
    size = 0x10000;
    status = NtAllocateVirtualMemory(&base, 0, &size,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    GEN_CHECK(status, STATUS_SUCCESS, "alloc for decommit");

    if (NT_SUCCESS(status)) {
        // Decommit first page
        PVOID decommit_base = base;
        SIZE_T decommit_size = 0x1000;
        status = NtFreeVirtualMemory(&decommit_base, &decommit_size, MEM_DECOMMIT);
        GEN_CHECK(status, STATUS_SUCCESS, "MEM_DECOMMIT page");

        // Query: first page should be reserved but not committed
        MEMORY_BASIC_INFORMATION mbi;
        NtQueryVirtualMemory(base, &mbi);
        GEN_CHECK(mbi.State, MEM_RESERVE, "decommitted -> reserved");

        // Can re-commit the decommitted page
        PVOID recommit_base = base;
        SIZE_T recommit_size = 0x1000;
        status = NtAllocateVirtualMemory(&recommit_base, 0, &recommit_size,
            MEM_COMMIT, PAGE_READWRITE);
        GEN_CHECK(status, STATUS_SUCCESS, "recommit");

        // Release everything
        PVOID free_base = base;
        SIZE_T free_size = 0;
        NtFreeVirtualMemory(&free_base, &free_size, MEM_RELEASE);
    }

    TEST_END();
}
