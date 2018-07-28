#include <xboxkrnl/xboxkrnl.h>

#include "ke_assertions.h"
#include "output.h"

void test_KeAlertResumeThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeAlertThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeBoostPriorityThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeBugCheck(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeBugCheckEx(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeCancelTimer(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeConnectInterrupt(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeDelayExecutionThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeDisconnectInterrupt(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeEnterCriticalRegion(){
    const char* func_num = "0x0065";
    const char* func_name = "KeEnterCriticalRegion";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    PKTHREAD thread = KeGetCurrentThread();
    tests_passed &= assert_critical_region(thread, 0, "Before entering critical region");

    KeEnterCriticalRegion();
    tests_passed &= assert_critical_region(thread, -1, "In critical region");

    KeEnterCriticalRegion();
    tests_passed &= assert_critical_region(thread, -2, "Entered critical region again");

    KeLeaveCriticalRegion();
    KeLeaveCriticalRegion();
    tests_passed &= assert_critical_region(thread, 0, "Leave critical region twice");

    print_test_footer(func_num, func_name, tests_passed);
}

void test_KeGetCurrentIrql(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeGetCurrentThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInitializeApc(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInitializeDeviceQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInitializeDpc(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInitializeEvent(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInitializeInterrupt(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInitializeMutant(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInitializeQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInitializeSemaphore(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInitializeTimerEx(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInsertByKeyDeviceQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInsertDeviceQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInsertHeadQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInsertQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInsertQueueApc(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInsertQueueDpc(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInterruptTime(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeIsExecutingDpc(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeLeaveCriticalRegion(){
    const char* func_num = "0x007A";
    const char* func_name = "KeLeaveCriticalRegion";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    PKTHREAD thread = KeGetCurrentThread();
    KeEnterCriticalRegion();
    KeEnterCriticalRegion();

    KeLeaveCriticalRegion();
    tests_passed &= assert_critical_region(thread, -1, "Leave critical region after entering twice");

    KeLeaveCriticalRegion();
    tests_passed &= assert_critical_region(thread, 0, "Leave critical region again");

    print_test_footer(func_num, func_name, tests_passed);
}

void test_KePulseEvent(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeQueryBasePriorityThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeQueryInterruptTime(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeQueryPerformanceCounter(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeQueryPerformanceFrequency(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeQuerySystemTime(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRaiseIrqlToDpcLevel(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRaiseIrqlToSynchLevel(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeReleaseMutant(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeReleaseSemaphore(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRemoveByKeyDeviceQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRemoveDeviceQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRemoveEntryDeviceQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRemoveQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRemoveQueueDpc(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeResetEvent(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRestoreFloatingPointState(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeResumeThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRundownQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSaveFloatingPointState(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSetBasePriorityThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSetDisableBoostThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSetEvent(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSetEventBoostPriority(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSetPriorityProcess(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSetPriorityThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSetTimer(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSetTimerEx(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeStallExecutionProcessor(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSuspendThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSynchronizeExecution(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSystemTime(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeTestAlertThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeTickCount(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeTimeIncrement(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeWaitForMultipleObjects(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeWaitForSingleObject(){
    /* FIXME: This is a stub! implement this function! */
}
