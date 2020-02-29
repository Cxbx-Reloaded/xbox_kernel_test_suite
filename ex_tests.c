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
        print("ERROR: did not create thread");
        tests_passed = 0;
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }

    while(ReadWriteLock.LockCount != 1) {
        Sleep(10);
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
        Sleep(10);
    }

    print_test_footer(func_num, func_name, tests_passed);
}

static void ExAcquireReadWriteLockShared_thread2(void* arg) {
    control_struct* control = (control_struct*)arg;

    // Test case where LockCount != 0, and ReaderEntryCount == 0, which should cause this thread to wait.
    while(control->thread2_cmd != 1) {
        Sleep(10);
    }
    ExAcquireReadWriteLockShared(control->ReadWriteLock);
    control->thread2_status = 1;
    ExReleaseReadWriteLock(control->ReadWriteLock);
    control->thread2_status = 2;


    // Test case where LockCount != 0, ReaderEntryCount != 0, and WritersWaitingCount == 0. Should grab lock fine.
    while(control->thread2_cmd != 2) {
        Sleep(10);
    }
    ExAcquireReadWriteLockShared(control->ReadWriteLock);
    control->thread2_status = 3;

    while(control->thread2_cmd != 3) {
        Sleep(10);
    }
    ExReleaseReadWriteLock(control->ReadWriteLock);
    control->thread2_status = 4;


    // Test case where LockCount != 0, ReaderEntryCount != 0, and WritersWaitingCount != 0. Should wait for exclusive thread to release.
    while(control->thread2_cmd != 4) {
        Sleep(10);
    }
    ExAcquireReadWriteLockExclusive(control->ReadWriteLock);
    control->thread2_status = 5;
    ExReleaseReadWriteLock(control->ReadWriteLock);
    control->thread2_status = 6;
}

static void ExAcquireReadWriteLockShared_thread3(void* arg) {
    control_struct* control = (control_struct*)arg;

    ExAcquireReadWriteLockShared(control->ReadWriteLock);
    control->thread3_status = 1;
    ExReleaseReadWriteLock(control->ReadWriteLock);
    control->thread3_status = 2;
}

void test_ExAcquireReadWriteLockShared(){
    const char* func_num = "0x000D";
    const char* func_name = "ExAcquireReadWriteLockShared";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    ERWLOCK ReadWriteLock;
    ExInitializeReadWriteLock(&ReadWriteLock);


    // Test case where LockCount == 0. Should obtain the lock.
    ExAcquireReadWriteLockShared(&ReadWriteLock);
    tests_passed = assert_ERWLOCK_equals(
        &ReadWriteLock,
        0, 0, 0, 1,
        "Acquire shared lock on empty lock"
    );
    // Avoid spinning forever in the loop below.
    if(!tests_passed) {
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }
    ExReleaseReadWriteLock(&ReadWriteLock);


    // Test case where LockCount != 0, and ReaderEntryCount == 0, which should cause the other thread to wait.
    control_struct control = {&ReadWriteLock, 0, 0, 0, 0};
    HANDLE handle;
    NTSTATUS result = PsCreateSystemThread(&handle, NULL, ExAcquireReadWriteLockShared_thread2, (void*)&control, 0);
    if(result != STATUS_SUCCESS) {
        print("ERROR: Did not create thread2");
        tests_passed = 0;
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }

    ExAcquireReadWriteLockExclusive(&ReadWriteLock);
    control.thread2_cmd = 1;

    while(ReadWriteLock.LockCount != 1) {
        Sleep(10);
    }
    tests_passed = assert_ERWLOCK_equals(
        &ReadWriteLock,
        1, 0, 1, 0,
        "Second thread attempted to acquire the exclusive lock, incrementing ReadersWaitingCount and waiting"
    );
    if(control.thread2_status == 1) {
        tests_passed = 0;
        print("    ERROR: The second thread was not supposed to write before the lock is released on the first thread.");
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }

    ExReleaseReadWriteLock(&ReadWriteLock);
    while(control.thread2_status != 2) {
        Sleep(10);
    }

    // Test case where LockCount != 0, ReaderEntryCount != 0, and WritersWaitingCount == 0. Should grab lock fine.
    ExAcquireReadWriteLockShared(&ReadWriteLock);
    control.thread2_cmd = 2;

    while(ReadWriteLock.LockCount != 1) {
        Sleep(1);
    }
    tests_passed = assert_ERWLOCK_equals(
        &ReadWriteLock,
        1, 0, 0, 2,
        "Second thread attempted to acquire the shared lock, incrementing ReaderEntryCount and getting the lock"
    );
    Sleep(10);
    if(control.thread2_status != 3) {
        tests_passed = 0;
        print("    ERROR: The second thread was supposed to obtain the shared lock and update thread2_status.");
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }

    control.thread2_cmd = 3;
    ExReleaseReadWriteLock(&ReadWriteLock);
    while(control.thread2_status != 4) {
        Sleep(10);
    }

    // Test case where LockCount != 0, ReaderEntryCount != 0, and WritersWaitingCount != 0. Should wait for exclusive thread to release.
    ExAcquireReadWriteLockShared(&ReadWriteLock);
    control.thread2_cmd = 4;

    while(ReadWriteLock.LockCount != 1) {
        Sleep(10);
    }

    HANDLE handle_thread3;
    result = PsCreateSystemThread(&handle_thread3, NULL, ExAcquireReadWriteLockShared_thread3, (void*)&control, 0);
    if(result != STATUS_SUCCESS) {
        tests_passed = 0;
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }

    while(ReadWriteLock.LockCount != 2) {
        Sleep(10);
    }
    tests_passed = assert_ERWLOCK_equals(
        &ReadWriteLock,
        2, 1, 1, 1,
        "Third thread attempted to acquire the shared lock but there is already another exclusive request in flight."
    );
    if(control.thread3_status == 1) {
        tests_passed = 0;
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }
    ExReleaseReadWriteLock(&ReadWriteLock);

    while( (control.thread2_status != 6) || (control.thread3_status != 2) ) {
        Sleep(10);
    }

    print_test_footer(func_num, func_name, tests_passed);
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
