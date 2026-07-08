#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmDbgAllocateMemory)
{
    TEST_BEGIN();

    // MmDbgAllocateMemory is a DEVKIT-only function.
    // On retail, the kernel thunk pointer is NULL - calling it would crash.
    if ((void*)MmDbgAllocateMemory == NULL) {
        TEST_SKIP("thunk is NULL (retail)");
        TEST_END();
        return;
    }

    PVOID mem = MmDbgAllocateMemory(PAGE_SIZE, PAGE_READWRITE);
    if (mem) {
        BOOLEAN is_valid = MmIsAddressValid(mem);
        GEN_CHECK(is_valid, TRUE, "dbg memory valid");
        *(volatile ULONG*)mem = 0xDBDBDBDB;
        GEN_CHECK(*(volatile ULONG*)mem, 0xDBDBDBDB, "dbg memory writable");
        MmDbgFreeMemory(mem, PAGE_SIZE);
    }

    TEST_END();
}
