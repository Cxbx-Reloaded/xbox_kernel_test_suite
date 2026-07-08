#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmPersistContiguousMemory)
{
    TEST_BEGIN();

    // Allocate contiguous memory and mark as persistent
    PVOID mem = MmAllocateContiguousMemory(PAGE_SIZE);
    GEN_CHECK(mem != NULL, TRUE, "allocation");
    if (mem) {
        // Mark as persistent (survives soft reboot) - should not crash
        MmPersistContiguousMemory(mem, PAGE_SIZE, TRUE);
        GEN_CHECK(TRUE, TRUE, "persist TRUE completed");

        // Unmark persistence
        MmPersistContiguousMemory(mem, PAGE_SIZE, FALSE);
        GEN_CHECK(TRUE, TRUE, "persist FALSE completed");

        MmFreeContiguousMemory(mem);
    }

    TEST_END();
}
