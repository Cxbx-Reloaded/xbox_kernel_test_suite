#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmGetPhysicalAddress)
{
    TEST_BEGIN();

    // --- Stack variable has valid physical mapping ---
    volatile ULONG stack_var = 42;
    PHYSICAL_ADDRESS phys = MmGetPhysicalAddress((PVOID)&stack_var);
    GEN_CHECK(phys != 0, TRUE, "stack physical non-zero");
    GEN_CHECK(phys < 0x08000000, TRUE, "stack within 128MB");

    // --- Function pointer (code segment) ---
    PHYSICAL_ADDRESS code_phys = MmGetPhysicalAddress((PVOID)&MmGetPhysicalAddress);
    GEN_CHECK(code_phys != 0, TRUE, "code segment has physical addr");
    GEN_CHECK(code_phys < 0x08000000, TRUE, "code within 128MB");

    // --- Global data ---
    extern PLAUNCH_DATA_PAGE LaunchDataPage;
    PHYSICAL_ADDRESS global_phys = MmGetPhysicalAddress((PVOID)&LaunchDataPage);
    GEN_CHECK(global_phys != 0, TRUE, "global has physical addr");

    // --- Allocated memory ---
    PVOID mem = MmAllocateSystemMemory(PAGE_SIZE, PAGE_READWRITE);
    if (mem) {
        PHYSICAL_ADDRESS mem_phys = MmGetPhysicalAddress(mem);
        GEN_CHECK(mem_phys != 0, TRUE, "alloc physical non-zero");
        GEN_CHECK(mem_phys < 0x08000000, TRUE, "alloc within 128MB");

        // Adjacent bytes on same page should map to adjacent physical addresses
        PHYSICAL_ADDRESS phys0 = MmGetPhysicalAddress(mem);
        PHYSICAL_ADDRESS phys1 = MmGetPhysicalAddress((PUCHAR)mem + 1);
        GEN_CHECK(phys1 - phys0, 1, "adjacent bytes = adjacent phys");

        // Same page offset relationship for larger offsets
        PHYSICAL_ADDRESS phys100 = MmGetPhysicalAddress((PUCHAR)mem + 0x100);
        GEN_CHECK(phys100 - phys0, 0x100, "offset 0x100 correct");

        MmFreeSystemMemory(mem, PAGE_SIZE);
    }

    // --- Contiguous memory: physical addresses must be sequential ---
    mem = MmAllocateContiguousMemory(PAGE_SIZE * 2);
    if (mem) {
        PHYSICAL_ADDRESS p0 = MmGetPhysicalAddress(mem);
        PHYSICAL_ADDRESS p1 = MmGetPhysicalAddress((PUCHAR)mem + PAGE_SIZE);
        GEN_CHECK(p1 - p0, PAGE_SIZE, "contiguous pages sequential");
        MmFreeContiguousMemory(mem);
    }

    // --- Consistency: calling twice gives same result ---
    phys = MmGetPhysicalAddress((PVOID)&stack_var);
    PHYSICAL_ADDRESS phys2 = MmGetPhysicalAddress((PVOID)&stack_var);
    GEN_CHECK(phys, phys2, "consistent result");

    // --- NULL/invalid address ---
    // SKIP: MmGetPhysicalAddress on NULL or unmapped addresses may crash
    // or return 0. The function does not validate input on Xbox kernel.

    TEST_END();
}
