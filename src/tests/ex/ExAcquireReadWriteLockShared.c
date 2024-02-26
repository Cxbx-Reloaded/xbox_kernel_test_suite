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

static void increment_thread2_cmd(control_struct* control, const char* calling_func)
{
    control->thread2_cmd++;
    print("  %s: thread2_cmd = %u", calling_func, control->thread2_cmd);
}

static void increment_thread2_status(control_struct* control, const char* calling_func)
{
    control->thread2_status++;
    print("  %s: thread2_status = %u", calling_func, control->thread2_status);
}

static void increment_thread3_status(control_struct* control, const char* calling_func)
{
    control->thread3_status++;
    print("  %s: thread3_status = %u", calling_func, control->thread3_status);
}

static BOOL timed_poll_for_value(ULONG* poll_var, ULONG wait_value)
{
    for(BYTE i = 0; i < 10; i++) {
        if(*poll_var == wait_value) {
            return 1;
        }
        Sleep(10);
    }
    return 0;
}

static void NTAPI ExAcquireReadWriteLockShared_thread2(void* arg)
{
    control_struct* control = (control_struct*)arg;
    const char* api_name = "ExAcquireReadWriteLockShared_thread2";

    // Test case where LockCount != 0, and ReaderEntryCount == 0, which should cause this thread to wait.
    BOOL value_found = timed_poll_for_value(&control->thread2_cmd, 1);
    if(!value_found) {
        print("  ERROR: %s failed waiting for thread2_cmd == 1", api_name);
        PsTerminateSystemThread(STATUS_TIMEOUT);
    }
    ExAcquireReadWriteLockShared(control->ReadWriteLock);
    increment_thread2_status(control, api_name);
    ExReleaseReadWriteLock(control->ReadWriteLock);
    increment_thread2_status(control, api_name);


    // Test case where LockCount != 0, ReaderEntryCount != 0, and WritersWaitingCount == 0. Should grab lock fine.
    value_found = timed_poll_for_value(&control->thread2_cmd, 2);
    if(!value_found) {
        print("  ERROR: %s failed waiting for thread2_cmd == 2", api_name);
        PsTerminateSystemThread(STATUS_TIMEOUT);
    }
    ExAcquireReadWriteLockShared(control->ReadWriteLock);
    increment_thread2_status(control, api_name);

    value_found = timed_poll_for_value(&control->thread2_cmd, 3);
    if(!value_found) {
        print("  ERROR: %s failed waiting for thread2_cmd == 3", api_name);
        PsTerminateSystemThread(STATUS_TIMEOUT);
    }
    ExReleaseReadWriteLock(control->ReadWriteLock);
    increment_thread2_status(control, api_name);


    // Test case where LockCount != 0, ReaderEntryCount != 0, and WritersWaitingCount != 0. Should wait for exclusive thread to release.
    value_found = timed_poll_for_value(&control->thread2_cmd, 4);
    if(!value_found) {
        print("  ERROR: %s failed waiting for thread2_cmd == 4", api_name);
        PsTerminateSystemThread(STATUS_TIMEOUT);
    }
    ExAcquireReadWriteLockExclusive(control->ReadWriteLock);
    increment_thread2_status(control, api_name);
    ExReleaseReadWriteLock(control->ReadWriteLock);
    increment_thread2_status(control, api_name);
}

static void NTAPI ExAcquireReadWriteLockShared_thread3(void* arg)
{
    control_struct* control = (control_struct*)arg;
    const char* api_name = "ExAcquireReadWriteLockShared_thread3";

    ExAcquireReadWriteLockShared(control->ReadWriteLock);
    increment_thread3_status(control, api_name);
    ExReleaseReadWriteLock(control->ReadWriteLock);
    increment_thread3_status(control, api_name);
}

TEST_FUNC(ExAcquireReadWriteLockShared)
{
    TEST_BEGIN();

    ERWLOCK ReadWriteLock;
    ExInitializeReadWriteLock(&ReadWriteLock);


    // Test case where LockCount == 0. Should obtain the lock.
    ExAcquireReadWriteLockShared(&ReadWriteLock);
    // Acquire shared lock on empty lock
    assert_ERWLOCK_equals(&ReadWriteLock, 0, 0, 0, 1);
    // Avoid spinning forever in the loop below.
    if(TEST_IS_FAILED) {
        TEST_END();
        return;
    }
    ExReleaseReadWriteLock(&ReadWriteLock);


    // Test case where LockCount != 0, and ReaderEntryCount == 0, which should cause the other thread to wait.
    control_struct control = {&ReadWriteLock, 0, 0, 0, 0};
    HANDLE handle;
    NTSTATUS result = PsCreateSystemThread(&handle, NULL, ExAcquireReadWriteLockShared_thread2, (void*)&control, 0);
    if(result != STATUS_SUCCESS) {
        print("  ERROR: Did not create thread2");
        TEST_FAILED();
        TEST_END();
        return;
    }

    ExAcquireReadWriteLockExclusive(&ReadWriteLock);
    increment_thread2_cmd(&control, api_name);

    test_passed = timed_poll_for_value((ULONG*)&ReadWriteLock.LockCount, 1);
    if(TEST_IS_FAILED) {
        print("  ERROR: %s failed waiting for LockCount == 1", api_name);
        TEST_END();
        return;
    }
    // Second thread attempted to acquire the exclusive lock, incrementing ReadersWaitingCount and waiting
    assert_ERWLOCK_equals(&ReadWriteLock, 1, 0, 1, 0);
    if(control.thread2_status == 1) {
        TEST_FAILED();
        print("  ERROR: The second thread was not supposed to write before the lock is released on the first thread.");
        TEST_END();
        return;
    }

    ExReleaseReadWriteLock(&ReadWriteLock);
    test_passed = timed_poll_for_value(&control.thread2_status, 2);
    if(TEST_IS_FAILED) {
        print("  ERROR: %s failed waiting for thread2_status == 2", api_name);
        TEST_END();
        return;
    }

    // Test case where LockCount != 0, ReaderEntryCount != 0, and WritersWaitingCount == 0. Should grab lock fine.
    ExAcquireReadWriteLockShared(&ReadWriteLock);
    increment_thread2_cmd(&control, api_name);

    test_passed = timed_poll_for_value((ULONG*)&ReadWriteLock.LockCount, 1);
    if(TEST_IS_FAILED) {
        print("  ERROR: %s failed waiting for LockCount == 1", api_name);
        TEST_END();
        return;
    }
    // Second thread attempted to acquire the shared lock, incrementing ReaderEntryCount and getting the lock
    assert_ERWLOCK_equals(&ReadWriteLock, 1, 0, 0, 2);
    Sleep(10);
    if(control.thread2_status != 3) {
        TEST_FAILED();
        print("  ERROR: The second thread was supposed to obtain the shared lock and update thread2_status.");
        TEST_END();
        return;
    }

    increment_thread2_cmd(&control, api_name);
    ExReleaseReadWriteLock(&ReadWriteLock);
    test_passed = timed_poll_for_value(&control.thread2_status, 4);
    if(TEST_IS_FAILED) {
        print("  ERROR: %s failed waiting for thread2_status == 4", api_name);
        TEST_END();
        return;
    }

    // Test case where LockCount != 0, ReaderEntryCount != 0, and WritersWaitingCount != 0. Should wait for exclusive thread to release.
    ExAcquireReadWriteLockShared(&ReadWriteLock);
    increment_thread2_cmd(&control, api_name);

    test_passed = timed_poll_for_value((ULONG*)&ReadWriteLock.LockCount, 1);
    if(TEST_IS_FAILED) {
        print("  ERROR: %s failed waiting for LockCount == 1", api_name);
        TEST_END();
        return;
    }

    HANDLE handle_thread3;
    result = PsCreateSystemThread(&handle_thread3, NULL, ExAcquireReadWriteLockShared_thread3, (void*)&control, 0);
    if(result != STATUS_SUCCESS) {
        TEST_FAILED();
        TEST_END();
        return;
    }

    test_passed = timed_poll_for_value((ULONG*)&ReadWriteLock.LockCount, 2);
    if(TEST_IS_FAILED) {
        print("  ERROR: %s failed waiting for LockCount == 2", api_name);
        TEST_END();
        return;
    }
    // Third thread attempted to acquire the shared lock but there is already another exclusive request in flight.
    assert_ERWLOCK_equals(&ReadWriteLock, 2, 1, 1, 1);
    if(control.thread3_status == 1) {
        TEST_FAILED();
        TEST_END();
        return;
    }
    ExReleaseReadWriteLock(&ReadWriteLock);

    print("  Waiting for thread statuses to be at their final value.");
    for(BYTE i = 0; i < 10; i++) {
        TEST_FAILED(); // TODO: Why force fail here?
        if( (control.thread2_status == 6) && (control.thread3_status == 2) ) {
            test_passed = 1; // TODO: test_passed should not be used here.
            break;
        }
        Sleep(10);
    }

    TEST_END();
}
