#include <xboxkrnl/xboxkrnl.h>
#include <windows.h>

#include "ex_assertions.h"
#include "assertion_defines.h"
#include "output.h"

typedef struct {
    PERWLOCK ReadWriteLock;
    ULONG thread2_cmd;
    ULONG thread2_status;
    ULONG thread3_cmd;
    ULONG thread3_status;
} control_struct;

static void ExAcquireReadWriteLockExclusive_thread2(void* arg) {
    control_struct* control = (control_struct*)arg;
    ExAcquireReadWriteLockExclusive(control->ReadWriteLock);
    control->thread2_status = 1;
    ExReleaseReadWriteLock(control->ReadWriteLock);
    control->thread2_status = 2;
}

void test_ExAcquireReadWriteLockExclusive(){
    const char* func_num = "0x000C";
    const char* func_name = "ExAcquireReadWriteLockExclusive";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    ERWLOCK ReadWriteLock;
    ExInitializeReadWriteLock(&ReadWriteLock);

    ExAcquireReadWriteLockExclusive(&ReadWriteLock);
    tests_passed = assert_ERWLOCK_equals(
        &ReadWriteLock,
        0, 0, 0, 0,
        "Acquire exclusive lock on empty lock"
    );
    // Avoid spinning forever in the loop below.
    if(!tests_passed) {
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }
    ExReleaseReadWriteLock(&ReadWriteLock);


    // Test case where one thread has the lock exclusively and another thread requests it exclusive. Should wait.
    ExAcquireReadWriteLockExclusive(&ReadWriteLock);

    HANDLE handle;
    control_struct control = {&ReadWriteLock, 0, 0, 0, 0};
    NTSTATUS result = PsCreateSystemThread(&handle, NULL, ExAcquireReadWriteLockExclusive_thread2, (void*)&control, 0);
    if(result != STATUS_SUCCESS) {
        print("ERROR: did not create thread\n");
        tests_passed = 0;
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }

    while(ReadWriteLock.LockCount != 1) {
        Sleep(100);
    }
    tests_passed = assert_ERWLOCK_equals(
        &ReadWriteLock,
        1, 1, 0, 0,
        "Second thread attempted to acquire the exclusive lock, incrementing WritersWaitingCount"
    );

    if(control.thread2_status == 1) {
        tests_passed = 0;
        print("    ERROR: The second thread was not supposed to write before the lock is released on the first thread.");
    }
    ExReleaseReadWriteLock(&ReadWriteLock);
    while(control.thread2_status != 2) {
        Sleep(100);
    }

    print_test_footer(func_num, func_name, tests_passed);
}

void test_ExAcquireReadWriteLockShared(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExAllocatePool(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExAllocatePoolWithTag(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExEventObjectType(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExFreePool(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExInitializeReadWriteLock(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExInterlockedAddLargeInteger(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExInterlockedAddLargeStatistic(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExInterlockedCompareExchange64(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExMutantObjectType(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExQueryPoolBlockSize(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExQueryNonVolatileSetting(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExReadWriteRefurbInfo(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExRaiseException(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExRaiseStatus(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExReleaseReadWriteLock(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExSaveNonVolatileSetting(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExSemaphoreObjectType(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExTimerObjectType(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExfInterlockedInsertHeadList(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExfInterlockedInsertTailList(){
    /* FIXME: This is a stub! implement this function! */
}

void test_ExfInterlockedRemoveHeadList(){
    /* FIXME: This is a stub! implement this function! */
}
