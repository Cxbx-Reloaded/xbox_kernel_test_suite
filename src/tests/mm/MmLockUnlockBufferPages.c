#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmLockUnlockBufferPages)
{
    TEST_BEGIN();

    // Allocate memory, lock it, unlock it
    PVOID mem = MmAllocateSystemMemory(PAGE_SIZE, PAGE_READWRITE);
    GEN_CHECK(mem != NULL, TRUE, "allocation");
    if (mem) {
        // Lock the pages (should not crash)
        MmLockUnlockBufferPages(mem, PAGE_SIZE, FALSE); // FALSE = lock
        GEN_CHECK(TRUE, TRUE, "lock completed");

        // Unlock the pages
        MmLockUnlockBufferPages(mem, PAGE_SIZE, TRUE); // TRUE = unlock
        GEN_CHECK(TRUE, TRUE, "unlock completed");

        MmFreeSystemMemory(mem, PAGE_SIZE);
    }

    TEST_END();
}
