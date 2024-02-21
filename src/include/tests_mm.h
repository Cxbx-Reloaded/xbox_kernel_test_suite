#pragma once

TEST_FUNC(MmGlobalData);
TEST_FUNC(MmAllocateContiguousMemory);
TEST_FUNC(MmAllocateContiguousMemoryEx);
TEST_FUNC(MmAllocateSystemMemory);
TEST_FUNC(MmClaimGpuInstanceMemory);
TEST_FUNC(MmCreateKernelStack);
TEST_FUNC(MmDeleteKernelStack);
TEST_FUNC(MmFreeContiguousMemory);
TEST_FUNC(MmFreeSystemMemory);
TEST_FUNC(MmGetPhysicalAddress);
TEST_FUNC(MmIsAddressValid);
TEST_FUNC(MmLockUnlockBufferPages);
TEST_FUNC(MmLockUnlockPhysicalPage);
TEST_FUNC(MmMapIoSpace);
TEST_FUNC(MmPersistContiguousMemory);
TEST_FUNC(MmQueryAddressProtect);
TEST_FUNC(MmQueryAllocationSize);
TEST_FUNC(MmQueryStatistics);
TEST_FUNC(MmSetAddressProtect);
TEST_FUNC(MmUnmapIoSpace);
TEST_FUNC(MmDbgAllocateMemory);
TEST_FUNC(MmDbgFreeMemory);
TEST_FUNC(MmDbgQueryAvailablePages);
TEST_FUNC(MmDbgReleaseAddress);
TEST_FUNC(MmDbgWriteCheck);
