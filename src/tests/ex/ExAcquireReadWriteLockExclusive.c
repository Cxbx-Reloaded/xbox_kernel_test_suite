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

static void increment_thread3_cmd(control_struct* control, const char* calling_func)
{
    control->thread3_cmd++;
    print("  %s: thread3_cmd = %u", calling_func, control->thread3_cmd);
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

static void NTAPI ExAcquireReadWriteLockExclusive_thread2(void* arg)
{
    const char* func_name = "ExAcquireReadWriteLockExclusive_thread2";
    control_struct* control = (control_struct*)arg;
    ExAcquireReadWriteLockExclusive(control->ReadWriteLock);
    increment_thread2_status(control, func_name);
    ExReleaseReadWriteLock(control->ReadWriteLock);
    increment_thread2_status(control, func_name);
}

void test_ExAcquireReadWriteLockExclusive()
{
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
