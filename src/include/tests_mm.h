#pragma once

void test_MmGlobalData(int, const char*);
void test_MmAllocateContiguousMemory(int, const char*);
void test_MmAllocateContiguousMemoryEx(int, const char*);
void test_MmAllocateSystemMemory(int, const char*);
void test_MmClaimGpuInstanceMemory(int, const char*);
void test_MmCreateKernelStack(int, const char*);
void test_MmDeleteKernelStack(int, const char*);
void test_MmFreeContiguousMemory(int, const char*);
void test_MmFreeSystemMemory(int, const char*);
void test_MmGetPhysicalAddress(int, const char*);
void test_MmIsAddressValid(int, const char*);
void test_MmLockUnlockBufferPages(int, const char*);
void test_MmLockUnlockPhysicalPage(int, const char*);
void test_MmMapIoSpace(int, const char*);
void test_MmPersistContiguousMemory(int, const char*);
void test_MmQueryAddressProtect(int, const char*);
void test_MmQueryAllocationSize(int, const char*);
void test_MmQueryStatistics(int, const char*);
void test_MmSetAddressProtect(int, const char*);
void test_MmUnmapIoSpace(int, const char*);
void test_MmDbgAllocateMemory(int, const char*);
void test_MmDbgFreeMemory(int, const char*);
void test_MmDbgQueryAvailablePages(int, const char*);
void test_MmDbgReleaseAddress(int, const char*);
void test_MmDbgWriteCheck(int, const char*);
