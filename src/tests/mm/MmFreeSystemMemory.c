#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmFreeSystemMemory)
{
    TEST_BEGIN();

    // NOTE: these tests could be marked as failed if not enough memory can be allocated for them

    ULONG num_of_pages;

    // Test #1 - free one page
    PVOID sys_addr = MmAllocateSystemMemory(PAGE_SIZE, PAGE_READONLY);
    GEN_CHECK(sys_addr != NULL, TRUE, "SystemMemoryAddress");
    if (sys_addr) {
        num_of_pages = MmFreeSystemMemory(sys_addr, 0);
        GEN_CHECK(num_of_pages, 1, "num_of_pages");
    }

    // Test #2 - free two pages
    sys_addr = MmAllocateSystemMemory(PAGE_SIZE + 1, PAGE_READONLY);
    GEN_CHECK(sys_addr != NULL, TRUE, "SystemMemoryAddress");
    if (sys_addr) {
        num_of_pages = MmFreeSystemMemory(sys_addr, 0);
        GEN_CHECK(num_of_pages, 2, "num_of_pages");
    }

    TEST_END();
}
