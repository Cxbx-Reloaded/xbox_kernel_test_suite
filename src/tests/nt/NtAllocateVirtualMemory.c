#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtAllocateVirtualMemory)
{
    TEST_BEGIN();

    // --- Reserve + commit in one call ---
    PVOID base = NULL;
    SIZE_T size = 0x10000; // 64KB
    NTSTATUS status = NtAllocateVirtualMemory(&base, 0, &size,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    GEN_CHECK(status, STATUS_SUCCESS, "reserve+commit 64KB");
    GEN_CHECK(base != NULL, TRUE, "base returned");
    GEN_CHECK(size >= 0x10000, TRUE, "size >= requested");

    if (NT_SUCCESS(status)) {
        // Memory should be accessible and zeroed
        volatile ULONG* ptr = (volatile ULONG*)base;
        GEN_CHECK(ptr[0], 0, "memory zeroed");
        ptr[0] = 0xDEADBEEF;
        GEN_CHECK(ptr[0], 0xDEADBEEF, "memory writable");

        // Free it
        PVOID free_base = base;
        SIZE_T free_size = 0;
        status = NtFreeVirtualMemory(&free_base, &free_size, MEM_RELEASE);
        GEN_CHECK(status, STATUS_SUCCESS, "free");
    }

    // --- Reserve only, then commit ---
    base = NULL;
    size = 0x20000; // 128KB
    status = NtAllocateVirtualMemory(&base, 0, &size, MEM_RESERVE, PAGE_READWRITE);
    GEN_CHECK(status, STATUS_SUCCESS, "reserve only");

    if (NT_SUCCESS(status)) {
        // Commit a sub-region
        PVOID commit_base = base;
        SIZE_T commit_size = 0x1000; // 4KB
        status = NtAllocateVirtualMemory(&commit_base, 0, &commit_size,
            MEM_COMMIT, PAGE_READWRITE);
        GEN_CHECK(status, STATUS_SUCCESS, "commit subregion");

        if (NT_SUCCESS(status)) {
            volatile BYTE* p = (volatile BYTE*)commit_base;
            p[0] = 0x42;
            GEN_CHECK(p[0], 0x42, "committed page writable");
        }

        // Release the whole reservation
        PVOID free_base = base;
        SIZE_T free_size = 0;
        NtFreeVirtualMemory(&free_base, &free_size, MEM_RELEASE);
    }

    // --- PAGE_READONLY ---
    base = NULL;
    size = 0x1000;
    status = NtAllocateVirtualMemory(&base, 0, &size,
        MEM_RESERVE | MEM_COMMIT, PAGE_READONLY);
    GEN_CHECK(status, STATUS_SUCCESS, "readonly alloc");
    if (NT_SUCCESS(status)) {
        // Can read but not write (writing would fault - SKIP)
        volatile ULONG* p = (volatile ULONG*)base;
        GEN_CHECK(p[0], 0, "readonly readable");

        PVOID free_base = base;
        SIZE_T free_size = 0;
        NtFreeVirtualMemory(&free_base, &free_size, MEM_RELEASE);
    }

    TEST_END();
}
