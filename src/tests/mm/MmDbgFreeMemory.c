#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmDbgFreeMemory)
{
    TEST_BEGIN();

    // DEVKIT-only. On retail, the kernel thunk pointer is NULL.
    if ((void*)MmDbgFreeMemory == NULL) {
        TEST_SKIP("thunk is NULL (retail)");
        TEST_END();
        return;
    }

    PVOID mem = MmDbgAllocateMemory(PAGE_SIZE, PAGE_READWRITE);
    if (mem) {
        ULONG pages_freed = MmDbgFreeMemory(mem, PAGE_SIZE);
        GEN_CHECK(pages_freed >= 1, TRUE, "freed at least 1 page");
    }

    TEST_END();
}
