#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

void test_MmFreeSystemMemory()
{
    const char* func_num = "0x00AC";
    const char* func_name = "MmFreeSystemMemory";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

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

    print_test_footer(func_num, func_name, test_passed);
}
