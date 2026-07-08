#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmLockUnlockPhysicalPage)
{
    TEST_BEGIN();

    // Get a physical page from allocated memory
    PVOID mem = MmAllocateSystemMemory(PAGE_SIZE, PAGE_READWRITE);
    GEN_CHECK(mem != NULL, TRUE, "allocation");
    if (mem) {
        PHYSICAL_ADDRESS phys = MmGetPhysicalAddress(mem);
        ULONG pfn = phys / PAGE_SIZE;

        // Lock the physical page
        MmLockUnlockPhysicalPage(pfn, FALSE); // FALSE = lock
        GEN_CHECK(TRUE, TRUE, "physical lock completed");

        // Unlock the physical page
        MmLockUnlockPhysicalPage(pfn, TRUE); // TRUE = unlock
        GEN_CHECK(TRUE, TRUE, "physical unlock completed");

        MmFreeSystemMemory(mem, PAGE_SIZE);
    }

    TEST_END();
}
