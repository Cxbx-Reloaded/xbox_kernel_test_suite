#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmQueryAddressProtect)
{
    TEST_BEGIN();

    // --- PAGE_READWRITE ---
    PVOID mem = MmAllocateSystemMemory(PAGE_SIZE, PAGE_READWRITE);
    GEN_CHECK(mem != NULL, TRUE, "RW allocation");
    if (mem) {
        ULONG protect = MmQueryAddressProtect(mem);
        GEN_CHECK(protect, PAGE_READWRITE, "query returns PAGE_READWRITE");
        MmFreeSystemMemory(mem, PAGE_SIZE);
    }

    // --- PAGE_READONLY ---
    mem = MmAllocateSystemMemory(PAGE_SIZE, PAGE_READONLY);
    GEN_CHECK(mem != NULL, TRUE, "RO allocation");
    if (mem) {
        ULONG protect = MmQueryAddressProtect(mem);
        GEN_CHECK(protect, PAGE_READONLY, "query returns PAGE_READONLY");
        MmFreeSystemMemory(mem, PAGE_SIZE);
    }

    // --- PAGE_READWRITE | PAGE_NOCACHE ---
    mem = MmAllocateSystemMemory(PAGE_SIZE, PAGE_READWRITE | PAGE_NOCACHE);
    if (mem) {
        ULONG protect = MmQueryAddressProtect(mem);
        GEN_CHECK((protect & PAGE_READWRITE), PAGE_READWRITE, "nocache has RW");
        GEN_CHECK((protect & PAGE_NOCACHE), PAGE_NOCACHE, "nocache flag set");
        MmFreeSystemMemory(mem, PAGE_SIZE);
    }

    // --- PAGE_READWRITE | PAGE_WRITECOMBINE ---
    mem = MmAllocateSystemMemory(PAGE_SIZE, PAGE_READWRITE | PAGE_WRITECOMBINE);
    if (mem) {
        ULONG protect = MmQueryAddressProtect(mem);
        GEN_CHECK((protect & PAGE_READWRITE), PAGE_READWRITE, "writecombine has RW");
        GEN_CHECK((protect & PAGE_WRITECOMBINE), PAGE_WRITECOMBINE, "writecombine flag");
        MmFreeSystemMemory(mem, PAGE_SIZE);
    }

    // --- After MmSetAddressProtect, query reflects new value ---
    mem = MmAllocateSystemMemory(PAGE_SIZE, PAGE_READWRITE);
    if (mem) {
        MmSetAddressProtect(mem, PAGE_SIZE, PAGE_READONLY);
        ULONG protect = MmQueryAddressProtect(mem);
        GEN_CHECK(protect, PAGE_READONLY, "changed to READONLY");
        MmSetAddressProtect(mem, PAGE_SIZE, PAGE_READWRITE);
        MmFreeSystemMemory(mem, PAGE_SIZE);
    }

    // --- Code segment should be PAGE_EXECUTE_READ or similar ---
    ULONG code_protect = MmQueryAddressProtect((PVOID)&MmQueryAddressProtect);
    // Code is typically execute+read
    GEN_CHECK(code_protect != 0, TRUE, "code protection non-zero");

    // --- Stack should be PAGE_READWRITE ---
    volatile ULONG stack_var = 0;
    ULONG stack_protect = MmQueryAddressProtect((PVOID)&stack_var);
    GEN_CHECK(stack_protect, PAGE_READWRITE, "stack is PAGE_READWRITE");

    // --- NULL/unmapped ---
    // SKIP: MmQueryAddressProtect on unmapped addresses may bugcheck
    // or return PAGE_NOACCESS. Behavior is implementation-defined.

    TEST_END();
}
