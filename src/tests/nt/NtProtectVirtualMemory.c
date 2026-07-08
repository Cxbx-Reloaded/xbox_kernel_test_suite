#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtProtectVirtualMemory)
{
    TEST_BEGIN();

    // Allocate RW memory
    PVOID base = NULL;
    SIZE_T size = 0x1000;
    NTSTATUS status = NtAllocateVirtualMemory(&base, 0, &size,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    GEN_CHECK(status, STATUS_SUCCESS, "alloc RW");

    if (NT_SUCCESS(status)) {
        // Write to verify it's RW
        volatile ULONG* ptr = (volatile ULONG*)base;
        ptr[0] = 0xCAFE;
        GEN_CHECK(ptr[0], 0xCAFE, "initially writable");

        // --- Change to PAGE_READONLY ---
        PVOID protect_base = base;
        SIZE_T protect_size = 0x1000;
        ULONG old_protect = 0;
        status = NtProtectVirtualMemory(&protect_base, &protect_size,
            PAGE_READONLY, &old_protect);
        GEN_CHECK(status, STATUS_SUCCESS, "protect READONLY");
        GEN_CHECK(old_protect, PAGE_READWRITE, "old was READWRITE");

        // Can still read
        GEN_CHECK(ptr[0], 0xCAFE, "still readable after protect");

        // --- Change back to RW ---
        protect_base = base;
        protect_size = 0x1000;
        status = NtProtectVirtualMemory(&protect_base, &protect_size,
            PAGE_READWRITE, &old_protect);
        GEN_CHECK(status, STATUS_SUCCESS, "protect back to RW");
        GEN_CHECK(old_protect, PAGE_READONLY, "old was READONLY");

        // Write again to verify
        ptr[0] = 0xBEEF;
        GEN_CHECK(ptr[0], 0xBEEF, "writable again");

        // --- Change to PAGE_EXECUTE_READWRITE ---
        protect_base = base;
        protect_size = 0x1000;
        status = NtProtectVirtualMemory(&protect_base, &protect_size,
            PAGE_EXECUTE_READWRITE, &old_protect);
        GEN_CHECK(status, STATUS_SUCCESS, "protect XRW");

        // Clean up
        PVOID free_base = base;
        SIZE_T free_size = 0;
        NtFreeVirtualMemory(&free_base, &free_size, MEM_RELEASE);
    }

    TEST_END();
}
