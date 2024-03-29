#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

#define SYSTEM_MEMORY_BASE    0xD0000000
#define SYSTEM_MEMORY_SIZE    (512 * 1024 * 1024) // = 0x20000000
#define SYSTEM_MEMORY_END     (SYSTEM_MEMORY_BASE + SYSTEM_MEMORY_SIZE - 1) // 0xEFFFFFFF
#define IS_SYSTEM_ADDRESS(Va) (((ULONG)(Va) - SYSTEM_MEMORY_BASE) <= (SYSTEM_MEMORY_END - SYSTEM_MEMORY_BASE))

#define CHECK_ALIGNMENT(size, alignment) (((size) % (alignment)) == 0)

TEST_FUNC(MmAllocateSystemMemory)
{
    TEST_BEGIN();

   // NOTE: these tests could be marked as failed if not enough memory can be allocated for them

    ULONG num_of_pages;
    BOOL is_system_address;
    BOOL is_page_alignment;

    // Test #1 - RW and no cache
    PVOID sys_addr = MmAllocateSystemMemory(100, PAGE_READWRITE | PAGE_NOCACHE);
    GEN_CHECK(sys_addr != NULL, TRUE, "SystemMemoryAddress");
    if (sys_addr) {
        is_system_address = IS_SYSTEM_ADDRESS(sys_addr);
        GEN_CHECK(is_system_address, TRUE, "is_system_address");

        is_page_alignment = CHECK_ALIGNMENT((ULONG)sys_addr, PAGE_SIZE);
        GEN_CHECK(is_page_alignment, TRUE, "is_page_alignment");

        num_of_pages = MmFreeSystemMemory(sys_addr, 100);
        GEN_CHECK(num_of_pages, 1, "num_of_pages");
    }

    // Test #2 - expected failure to allocate memory
    sys_addr = MmAllocateSystemMemory(1, PAGE_READWRITE | PAGE_READONLY);
    GEN_CHECK(sys_addr != NULL, FALSE, "SystemMemoryAddress");
    if (sys_addr) {
        MmFreeSystemMemory(sys_addr, 0);
    }
    sys_addr = MmAllocateSystemMemory(1, PAGE_READWRITE | PAGE_EXECUTE);
    GEN_CHECK(sys_addr != NULL, FALSE, "SystemMemoryAddress");
    if (sys_addr) {
        MmFreeSystemMemory(sys_addr, 0);
    }
    sys_addr = MmAllocateSystemMemory(1, PAGE_READWRITE | PAGE_NOCACHE | PAGE_WRITECOMBINE);
    GEN_CHECK(sys_addr != NULL, FALSE, "SystemMemoryAddress");
    if (sys_addr) {
        MmFreeSystemMemory(sys_addr, 0);
    }

    TEST_END();
}
