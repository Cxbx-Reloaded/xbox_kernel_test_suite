#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/rtl.h"
#include "assertions/ke.h"

void test_RtlEnterCriticalSection(){
    const char* func_num = "0x0115";
    const char* func_name = "RtlEnterCriticalSection";
    RTL_CRITICAL_SECTION crit_section;
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    RtlInitializeCriticalSection(&crit_section);
    RtlEnterCriticalSection(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        0,
        1,
        (HANDLE)KeGetCurrentThread(),
        "Enter an unused critical section"
    );

    RtlEnterCriticalSection(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        1,
        2,
        (HANDLE)KeGetCurrentThread(),
        "Enter the critical section again"
    );

    RtlLeaveCriticalSection(&crit_section);
    RtlLeaveCriticalSection(&crit_section);
    RtlEnterCriticalSection(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        0,
        1,
        (HANDLE)KeGetCurrentThread(),
        "Re-Enter critical section after leaving twice"
    );

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlEnterCriticalSectionAndRegion(){
    const char* func_num = "0x0116";
    const char* func_name = "RtlEnterCriticalSectionAndRegion";
    RTL_CRITICAL_SECTION crit_section;
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    PKTHREAD thread = KeGetCurrentThread();
    const ULONG orig_APC_disable = thread->KernelApcDisable;

    RtlInitializeCriticalSection(&crit_section);
    RtlEnterCriticalSectionAndRegion(&crit_section);
    RtlEnterCriticalSectionAndRegion(&crit_section);

    tests_passed &= assert_critical_section_equals(
        &crit_section,
        1,
        2,
        (HANDLE)thread,
        "Enter critcal section and region twice"
    );
    tests_passed &= assert_critical_region(thread, orig_APC_disable - 2, "Entered critical region twice");

    // Cleanup machine state without using RtlLeaveCriticalSectionAndRegion because it only
    // increments KernelApcDisable when the critical region is freed (RecursionCount == 0)
    KeLeaveCriticalRegion();
    KeLeaveCriticalRegion();

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlInitializeCriticalSection(){
    const char* func_num = "0x0123";
    const char* func_name = "RtlInitializeCriticalSection";
    RTL_CRITICAL_SECTION crit_section;
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    RtlInitializeCriticalSection(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        -1,
        0,
        NULL,
        "Init critical section"
    );

    memset(&crit_section, 0x11, sizeof(crit_section));
    RtlInitializeCriticalSection(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        -1,
        0,
        NULL,
        "Re-Init critical section after setting garbage data"
    );

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlLeaveCriticalSection(){
    const char* func_num = "0x0126";
    const char* func_name = "RtlLeaveCriticalSection";
    RTL_CRITICAL_SECTION crit_section;
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    RtlInitializeCriticalSection(&crit_section);
    RtlEnterCriticalSection(&crit_section);
    RtlEnterCriticalSection(&crit_section);
    RtlLeaveCriticalSection(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        0,
        1,
        (HANDLE)KeGetCurrentThread(),
        "Leave critical section once"
    );

    RtlLeaveCriticalSection(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        -1,
        0,
        NULL,
        "Leave critical section again"
    );

    RtlEnterCriticalSection(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        0,
        1,
        (HANDLE)KeGetCurrentThread(),
        "Re-Enter Critical Section"
    );

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlLeaveCriticalSectionAndRegion(){
    const char* func_num = "0x0127";
    const char* func_name = "RtlLeaveCriticalSectionAndRegion";
    RTL_CRITICAL_SECTION crit_section;
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    PKTHREAD thread = KeGetCurrentThread();
    const ULONG orig_APC_disable = thread->KernelApcDisable;

    RtlInitializeCriticalSection(&crit_section);
    RtlEnterCriticalSectionAndRegion(&crit_section);
    RtlEnterCriticalSectionAndRegion(&crit_section);

    RtlLeaveCriticalSectionAndRegion(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        0,
        1,
        (HANDLE)thread,
        "Leave critical section and region after entering twice"
    );
    tests_passed &= assert_critical_region(thread, orig_APC_disable - 2, "KernelApcDisable should be unchanged until critical section is freed");

    RtlLeaveCriticalSectionAndRegion(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        -1,
        0,
        NULL,
        "Leave critical section and region a second time"
    );
    tests_passed &= assert_critical_region(thread, orig_APC_disable - 1, "KernelApcDisable should now decrement as the critial section is freed");

    // Cleanup machine state
    KeLeaveCriticalRegion();

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlTryEnterCriticalSection(){
    /* FIXME: This is a stub! implement this function! */
}
