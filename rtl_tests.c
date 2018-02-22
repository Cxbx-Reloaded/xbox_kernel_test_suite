#include <pbkit/pbkit.h>
#include <string.h>

#include "output.h"

static BOOL assert_critical_section_equals(
    PRTL_CRITICAL_SECTION crit_section,
    LONG expected_LockCount,
    LONG expected_RecursionCount,
    HANDLE expected_OwningThread,
    const char* test_name
)
{
    BOOL test_passed = 1;
    if(crit_section->LockCount != expected_LockCount) {
        print(
            "Expected LockCount = 0x%x, Got LockCount = 0x%x",
            expected_LockCount, crit_section->LockCount
        );
        test_passed = 0;
    }
    if(crit_section->RecursionCount != expected_RecursionCount) {
        print(
            "Expected RecursionCount = 0x%x, Got RecursionCount = 0x%x",
            expected_RecursionCount, crit_section->RecursionCount
        );
        test_passed = 0;
    }
    if(crit_section->OwningThread != expected_OwningThread) {
        print(
            "Expected OwningThread = 0x%x, Got OwningThread = 0x%x",
            expected_OwningThread, crit_section->OwningThread
        );
        test_passed = 0;
    }
    if(test_passed) {
        print("Test '%s' PASSED", test_name);
    }
    else {
        print("Test '%s' FAILED", test_name);
    }
    return test_passed;
}

void test_RtlAnsiStringToUnicodeString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlAppendStringToString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlAppendUnicodeStringToString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlAppendUnicodeToString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlAssert(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlCaptureContext(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlCaptureStackBackTrace(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlCharToInteger(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlCompareMemory(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlCompareMemoryUlong(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlCompareString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlCompareUnicodeString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlCopyString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlCopyUnicodeString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlCreateUnicodeString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlDowncaseUnicodeChar(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlDowncaseUnicodeString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlEnterCriticalSection(){
    const char* func_num = "0x0115";
    const char* func_name = "RtlEnterCriticalSection";
    RTL_CRITICAL_SECTION crit_section;
    BOOL tests_passed = 0;
    print_test_header(func_num, func_name);

    RtlInitializeCriticalSection(&crit_section);
    RtlEnterCriticalSection(&crit_section);
    tests_passed |= assert_critical_section_equals(
        &crit_section,
        0,
        1,
        (HANDLE)KeGetCurrentThread(),
        "Enter an unused critical section"
    );

    RtlEnterCriticalSection(&crit_section);
    tests_passed |= assert_critical_section_equals(
        &crit_section,
        1,
        2,
        (HANDLE)KeGetCurrentThread(),
        "Enter the critical section twice"
    );

    // Leave three times
    RtlLeaveCriticalSection(&crit_section);
    RtlLeaveCriticalSection(&crit_section);

    RtlEnterCriticalSection(&crit_section);
    tests_passed |= assert_critical_section_equals(
        &crit_section,
        0,
        1,
        (HANDLE)KeGetCurrentThread(),
        "Re-Enter critical section after leaving twice"
    );

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlEnterCriticalSectionAndRegion(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlEqualString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlEqualUnicodeString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlExtendedIntegerMultiply(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlExtendedLargeIntegerDivide(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlExtendedMagicDivide(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlFillMemory(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlFillMemoryUlong(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlFreeAnsiString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlFreeUnicodeString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlGetCallersAddress(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlInitAnsiString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlInitUnicodeString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlInitializeCriticalSection(){
    const char* func_num = "0x0123";
    const char* func_name = "RtlInitializeCriticalSection";
    RTL_CRITICAL_SECTION crit_section;
    BOOL tests_passed = 0;
    print_test_header(func_num, func_name);

    RtlInitializeCriticalSection(&crit_section);
    tests_passed |= assert_critical_section_equals(
        &crit_section,
        -1,
        0,
        NULL,
        "Init critical section"
    );

    memset(&crit_section, 0x11, sizeof(crit_section));
    RtlInitializeCriticalSection(&crit_section);
    tests_passed |= assert_critical_section_equals(
        &crit_section,
        -1,
        0,
        NULL,
        "Re-Init critical section after setting garbage data"
    );

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlIntegerToChar(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlIntegerToUnicodeString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlLeaveCriticalSection(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlLeaveCriticalSectionAndRegion(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlLowerChar(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlMapGenericMask(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlMoveMemory(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlMultiByteToUnicodeN(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlMultiByteToUnicodeSize(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlNtStatusToDosError(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlRaiseException(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlRaiseStatus(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlTimeFieldsToTime(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlTimeToTimeFields(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlTryEnterCriticalSection(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUlongByteSwap(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUnicodeStringToAnsiString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUnicodeStringToInteger(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUnicodeToMultiByteN(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUnicodeToMultiByteSize(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUnwind(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUpcaseUnicodeChar(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUpcaseUnicodeString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUpcaseUnicodeToMultiByteN(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUpperChar(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUpperString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUshortByteSwap(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlWalkFrameChain(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlZeroMemory(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlRip(){
    /* FIXME: This is a stub! implement this function! */
}
