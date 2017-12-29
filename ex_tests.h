#ifndef EX_TESTS_H
#define EX_TESTS_H

void test_ExAcquireReadWriteLockExclusive();
void test_ExAcquireReadWriteLockShared();
void test_ExAllocatePool();
void test_ExAllocatePoolWithTag();
void test_ExEventObjectType();
void test_ExFreePool();
void test_ExInitializeReadWriteLock();
void test_ExInterlockedAddLargeInteger();
void test_ExInterlockedAddLargeStatistic();
void test_ExInterlockedCompareExchange64();
void test_ExMutantObjectType();
void test_ExQueryPoolBlockSize();
void test_ExQueryNonVolatileSetting();
void test_ExReadWriteRefurbInfo();
void test_ExRaiseException();
void test_ExRaiseStatus();
void test_ExReleaseReadWriteLock();
void test_ExSaveNonVolatileSetting();
void test_ExSemaphoreObjectType();
void test_ExTimerObjectType();
void test_ExfInterlockedInsertHeadList();
void test_ExfInterlockedInsertTailList();
void test_ExfInterlockedRemoveHeadList();

#endif
