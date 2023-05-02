#include <xboxkrnl/xboxkrnl.h>
#include <windows.h>

#include "assertions/ex.h"
#include "assertions/defines.h"
#include "util/output.h"

typedef struct {
    PERWLOCK ReadWriteLock;
    ULONG thread2_cmd;
    ULONG thread2_status;
    ULONG thread3_cmd;
    ULONG thread3_status;
} control_struct;

static void increment_thread2_cmd(control_struct* control, const char* calling_func) {
    control->thread2_cmd++;
    print("  %s: thread2_cmd = %u", calling_func, control->thread2_cmd);
}

static void increment_thread2_status(control_struct* control, const char* calling_func) {
    control->thread2_status++;
    print("  %s: thread2_status = %u", calling_func, control->thread2_status);
}

static void increment_thread3_cmd(control_struct* control, const char* calling_func) {
    control->thread3_cmd++;
    print("  %s: thread3_cmd = %u", calling_func, control->thread3_cmd);
}

static void increment_thread3_status(control_struct* control, const char* calling_func) {
    control->thread3_status++;
    print("  %s: thread3_status = %u", calling_func, control->thread3_status);
}

static BOOL timed_poll_for_value(ULONG* poll_var, ULONG wait_value) {
    for(BYTE i = 0; i < 10; i++) {
        if(*poll_var == wait_value) {
            return 1;
        }
        Sleep(10);
    }
    return 0;
}

static void ExAcquireReadWriteLockExclusive_thread2(void* arg) {
    const char* func_name = "ExAcquireReadWriteLockExclusive_thread2";
    control_struct* control = (control_struct*)arg;
    ExAcquireReadWriteLockExclusive(control->ReadWriteLock);
    increment_thread2_status(control, func_name);
    ExReleaseReadWriteLock(control->ReadWriteLock);
    increment_thread2_status(control, func_name);
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
        print("  ERROR: did not create thread");
        tests_passed = 0;
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }

    tests_passed = timed_poll_for_value((ULONG*)&ReadWriteLock.LockCount, 1);
    if(!tests_passed) {
        print("  ERROR: LockCount did not equal 1\n");
        tests_passed = 0;
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }
    tests_passed = assert_ERWLOCK_equals(
        &ReadWriteLock,
        1, 1, 0, 0,
        "Second thread attempted to acquire the exclusive lock, incrementing WritersWaitingCount"
    );

    if(control.thread2_status == 1) {
        tests_passed = 0;
        print("  ERROR: The second thread was not supposed to write before the lock is released on the first thread.");
    }
    ExReleaseReadWriteLock(&ReadWriteLock);
    tests_passed = timed_poll_for_value(&control.thread2_status, 2);
    if(!tests_passed) {
        print("  ERROR: thread2_status did not equal 2 before timing out.\n");
        tests_passed = 0;
    }

    print_test_footer(func_num, func_name, tests_passed);
}

static void ExAcquireReadWriteLockShared_thread2(void* arg) {
    control_struct* control = (control_struct*)arg;
    const char* func_name = "ExAcquireReadWriteLockShared_thread2";

    // Test case where LockCount != 0, and ReaderEntryCount == 0, which should cause this thread to wait.
    BOOL value_found = timed_poll_for_value(&control->thread2_cmd, 1);
    if(!value_found) {
        print("  ERROR: %s failed waiting for thread2_cmd == 1", func_name);
        PsTerminateSystemThread(STATUS_TIMEOUT);
    }
    ExAcquireReadWriteLockShared(control->ReadWriteLock);
    increment_thread2_status(control, func_name);
    ExReleaseReadWriteLock(control->ReadWriteLock);
    increment_thread2_status(control, func_name);


    // Test case where LockCount != 0, ReaderEntryCount != 0, and WritersWaitingCount == 0. Should grab lock fine.
    value_found = timed_poll_for_value(&control->thread2_cmd, 2);
    if(!value_found) {
        print("  ERROR: %s failed waiting for thread2_cmd == 2", func_name);
        PsTerminateSystemThread(STATUS_TIMEOUT);
    }
    ExAcquireReadWriteLockShared(control->ReadWriteLock);
    increment_thread2_status(control, func_name);

    value_found = timed_poll_for_value(&control->thread2_cmd, 3);
    if(!value_found) {
        print("  ERROR: %s failed waiting for thread2_cmd == 3", func_name);
        PsTerminateSystemThread(STATUS_TIMEOUT);
    }
    ExReleaseReadWriteLock(control->ReadWriteLock);
    increment_thread2_status(control, func_name);


    // Test case where LockCount != 0, ReaderEntryCount != 0, and WritersWaitingCount != 0. Should wait for exclusive thread to release.
    value_found = timed_poll_for_value(&control->thread2_cmd, 4);
    if(!value_found) {
        print("  ERROR: %s failed waiting for thread2_cmd == 4", func_name);
        PsTerminateSystemThread(STATUS_TIMEOUT);
    }
    ExAcquireReadWriteLockExclusive(control->ReadWriteLock);
    increment_thread2_status(control, func_name);
    ExReleaseReadWriteLock(control->ReadWriteLock);
    increment_thread2_status(control, func_name);
}

static void ExAcquireReadWriteLockShared_thread3(void* arg) {
    control_struct* control = (control_struct*)arg;
    const char* func_name = "ExAcquireReadWriteLockShared_thread3";

    ExAcquireReadWriteLockShared(control->ReadWriteLock);
    increment_thread3_status(control, func_name);
    ExReleaseReadWriteLock(control->ReadWriteLock);
    increment_thread3_status(control, func_name);
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
        print("  ERROR: Did not create thread2");
        tests_passed = 0;
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }

    ExAcquireReadWriteLockExclusive(&ReadWriteLock);
    increment_thread2_cmd(&control, func_name);

    tests_passed = timed_poll_for_value((ULONG*)&ReadWriteLock.LockCount, 1);
    if(!tests_passed) {
        print("  ERROR: %s failed waiting for LockCount == 1", func_name);
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }
    tests_passed = assert_ERWLOCK_equals(
        &ReadWriteLock,
        1, 0, 1, 0,
        "Second thread attempted to acquire the exclusive lock, incrementing ReadersWaitingCount and waiting"
    );
    if(control.thread2_status == 1) {
        tests_passed = 0;
        print("  ERROR: The second thread was not supposed to write before the lock is released on the first thread.");
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }

    ExReleaseReadWriteLock(&ReadWriteLock);
    tests_passed = timed_poll_for_value(&control.thread2_status, 2);
    if(!tests_passed) {
        print("  ERROR: %s failed waiting for thread2_status == 2", func_name);
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }

    // Test case where LockCount != 0, ReaderEntryCount != 0, and WritersWaitingCount == 0. Should grab lock fine.
    ExAcquireReadWriteLockShared(&ReadWriteLock);
    increment_thread2_cmd(&control, func_name);

    tests_passed = timed_poll_for_value((ULONG*)&ReadWriteLock.LockCount, 1);
    if(!tests_passed) {
        print("  ERROR: %s failed waiting for LockCount == 1", func_name);
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }
    tests_passed = assert_ERWLOCK_equals(
        &ReadWriteLock,
        1, 0, 0, 2,
        "Second thread attempted to acquire the shared lock, incrementing ReaderEntryCount and getting the lock"
    );
    Sleep(10);
    if(control.thread2_status != 3) {
        tests_passed = 0;
        print("  ERROR: The second thread was supposed to obtain the shared lock and update thread2_status.");
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }

    increment_thread2_cmd(&control, func_name);
    ExReleaseReadWriteLock(&ReadWriteLock);
    tests_passed = timed_poll_for_value(&control.thread2_status, 4);
    if(!tests_passed) {
        print("  ERROR: %s failed waiting for thread2_status == 4", func_name);
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }

    // Test case where LockCount != 0, ReaderEntryCount != 0, and WritersWaitingCount != 0. Should wait for exclusive thread to release.
    ExAcquireReadWriteLockShared(&ReadWriteLock);
    increment_thread2_cmd(&control, func_name);

    tests_passed = timed_poll_for_value((ULONG*)&ReadWriteLock.LockCount, 1);
    if(!tests_passed) {
        print("  ERROR: %s failed waiting for LockCount == 1", func_name);
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }

    HANDLE handle_thread3;
    result = PsCreateSystemThread(&handle_thread3, NULL, ExAcquireReadWriteLockShared_thread3, (void*)&control, 0);
    if(result != STATUS_SUCCESS) {
        tests_passed = 0;
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }

    tests_passed = timed_poll_for_value((ULONG*)&ReadWriteLock.LockCount, 2);
    if(!tests_passed) {
        print("  ERROR: %s failed waiting for LockCount == 2", func_name);
        print_test_footer(func_num, func_name, tests_passed);
        return;
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

    print("  Waiting for thread statuses to be at their final value.");
    for(BYTE i = 0; i < 10; i++) {
        tests_passed = 0;
        if( (control.thread2_status == 6) && (control.thread3_status == 2) ) {
            tests_passed = 1;
            break;
        }
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
