#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"

#define PAGE_SIZE 4096

#define PAGE_NOACCESS          0x01
#define PAGE_READONLY          0x02
#define PAGE_READWRITE         0x04
#define PAGE_EXECUTE           0x10
#define PAGE_EXECUTE_READ      0x20
#define PAGE_EXECUTE_READWRITE 0x40
#define PAGE_GUARD             0x100
#define PAGE_NOCACHE           0x200
#define PAGE_WRITECOMBINE      0x400

#define SYSTEM_MEMORY_BASE    0xD0000000
#define SYSTEM_MEMORY_SIZE    (512 * 1024 * 1024) // = 0x20000000
#define SYSTEM_MEMORY_END     (SYSTEM_MEMORY_BASE + SYSTEM_MEMORY_SIZE - 1) // 0xEFFFFFFF
#define IS_SYSTEM_ADDRESS(Va) (((ULONG)(Va) - SYSTEM_MEMORY_BASE) <= (SYSTEM_MEMORY_END - SYSTEM_MEMORY_BASE))

#define CHECK_ALIGNMENT(size, alignment) (((size) % (alignment)) == 0)

void test_MmAllocateSystemMemory()
{
    const char* func_num = "0x00A7";
    const char* func_name = "MmAllocateSystemMemory";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    ULONG NumberOfPages;
    PVOID Addr = MmAllocateSystemMemory(100, PAGE_READWRITE | PAGE_NOCACHE);
    {
        if (Addr == NULL) {
            print("Failed to allocate system memory; %u\n", __LINE__);
            goto NextTest1;
        }
        if (IS_SYSTEM_ADDRESS(Addr) == FALSE) {
            print("Allocated system memory in unexpected region, Addr was %p; %u\n", Addr, __LINE__);
            tests_passed = 0;
            goto Free;
        }
        if (!CHECK_ALIGNMENT((ULONG)Addr, PAGE_SIZE)) {
            print("Addr is not aligned to a page boundary, Addr was %p; %u\n", Addr, __LINE__);
            tests_passed = 0;
            goto Free;
        }
    }
Free:
    NumberOfPages = MmFreeSystemMemory(Addr, 100);
    if (NumberOfPages != 1) {
        print("Unexpected number of pages freed, NumberOfPages was %u; %u\n", NumberOfPages, __LINE__);
        tests_passed = 0;
    }
NextTest1:
    Addr = MmAllocateSystemMemory(PAGE_SIZE, PAGE_READONLY);
    if (Addr == NULL) {
        print("Failed to allocate system memory; %u\n", __LINE__);
        goto NextTest2;
    }
    NumberOfPages = MmFreeSystemMemory(Addr, 0);
    if (NumberOfPages != 1) {
        print("Unexpected number of pages freed, NumberOfPages was %u; %u\n", NumberOfPages, __LINE__);
        tests_passed = 0;
    }
NextTest2:
    Addr = MmAllocateSystemMemory(PAGE_SIZE + 1, PAGE_READONLY);
    if (Addr == NULL) {
        print("Failed to allocate system memory; %u\n", __LINE__);
        goto NextTest3;
    }
    NumberOfPages = MmFreeSystemMemory(Addr, 0);
    if (NumberOfPages != 2) {
        print("Unexpected number of pages freed, NumberOfPages was %u; %u\n", NumberOfPages, __LINE__);
        tests_passed = 0;
    }
NextTest3:
    Addr = MmAllocateSystemMemory(1, PAGE_READWRITE | PAGE_READONLY);
    if (Addr != NULL) {
        print("Allocation unexpectedly succeeded; %u\n", __LINE__);
        MmFreeSystemMemory(Addr, 0);
        tests_passed = 0;
    }
    Addr = MmAllocateSystemMemory(1, PAGE_READWRITE | PAGE_EXECUTE);
    if (Addr != NULL) {
        print("Allocation unexpectedly succeeded; %u\n", __LINE__);
        MmFreeSystemMemory(Addr, 0);
        tests_passed = 0;
    }
    Addr = MmAllocateSystemMemory(1, PAGE_READWRITE | PAGE_NOCACHE | PAGE_WRITECOMBINE);
    if (Addr != NULL) {
        print("Allocation unexpectedly succeeded; %u\n", __LINE__);
        MmFreeSystemMemory(Addr, 0);
        tests_passed = 0;
    }

    print_test_footer(func_num, func_name, tests_passed);
}
