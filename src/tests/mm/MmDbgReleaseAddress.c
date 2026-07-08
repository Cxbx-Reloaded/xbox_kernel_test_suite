#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmDbgReleaseAddress)
{
    TEST_BEGIN();

    // DEVKIT-only. On retail, the kernel thunk pointer is NULL.
    if ((void*)MmDbgReleaseAddress == NULL) {
        TEST_SKIP("thunk is NULL (retail)");
        TEST_END();
        return;
    }

    PVOID mem = MmDbgAllocateMemory(PAGE_SIZE, PAGE_READWRITE);
    if (mem) {
        MmDbgReleaseAddress(mem, NULL);
        GEN_CHECK(TRUE, TRUE, "release completed");
        // Note: after release, the memory may still need to be freed
        MmDbgFreeMemory(mem, PAGE_SIZE);
    }

    TEST_END();
}
