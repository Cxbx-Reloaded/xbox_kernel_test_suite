#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/rtl.h"
#include "assertions/ke.h"

TEST_FUNC(RtlEnterCriticalSection)
{
    RTL_CRITICAL_SECTION crit_section;
    TEST_BEGIN();

    RtlInitializeCriticalSection(&crit_section);
    RtlEnterCriticalSection(&crit_section);
    test_passed &= assert_critical_section_equals(
        &crit_section,
        0,
        1,
        (HANDLE)KeGetCurrentThread(),
        "Enter an unused critical section"
    );

    RtlEnterCriticalSection(&crit_section);
    test_passed &= assert_critical_section_equals(
        &crit_section,
        1,
        2,
        (HANDLE)KeGetCurrentThread(),
        "Enter the critical section again"
    );

    RtlLeaveCriticalSection(&crit_section);
    RtlLeaveCriticalSection(&crit_section);
    RtlEnterCriticalSection(&crit_section);
    test_passed &= assert_critical_section_equals(
        &crit_section,
        0,
        1,
        (HANDLE)KeGetCurrentThread(),
        "Re-Enter critical section after leaving twice"
    );

    TEST_END();
}

TEST_FUNC(RtlEnterCriticalSectionAndRegion)
{
    RTL_CRITICAL_SECTION crit_section;
    TEST_BEGIN();

    PKTHREAD thread = KeGetCurrentThread();
    const ULONG orig_APC_disable = thread->KernelApcDisable;

    RtlInitializeCriticalSection(&crit_section);
    RtlEnterCriticalSectionAndRegion(&crit_section);
    RtlEnterCriticalSectionAndRegion(&crit_section);

    test_passed &= assert_critical_section_equals(
        &crit_section,
        1,
        2,
        (HANDLE)thread,
        "Enter critcal section and region twice"
    );
    test_passed &= assert_critical_region(thread, orig_APC_disable - 2, "Entered critical region twice");

    // Cleanup machine state without using RtlLeaveCriticalSectionAndRegion because it only
    // increments KernelApcDisable when the critical region is freed (RecursionCount == 0)
    KeLeaveCriticalRegion();
    KeLeaveCriticalRegion();

    TEST_END();
}

TEST_FUNC(RtlInitializeCriticalSection)
{
    RTL_CRITICAL_SECTION crit_section;
    TEST_BEGIN();

    RtlInitializeCriticalSection(&crit_section);
    test_passed &= assert_critical_section_equals(
        &crit_section,
        -1,
        0,
        NULL,
        "Init critical section"
    );

    memset(&crit_section, 0x11, sizeof(crit_section));
    RtlInitializeCriticalSection(&crit_section);
    test_passed &= assert_critical_section_equals(
        &crit_section,
        -1,
        0,
        NULL,
        "Re-Init critical section after setting garbage data"
    );

    TEST_END();
}

TEST_FUNC(RtlLeaveCriticalSection)
{
    RTL_CRITICAL_SECTION crit_section;
    TEST_BEGIN();

    RtlInitializeCriticalSection(&crit_section);
    RtlEnterCriticalSection(&crit_section);
    RtlEnterCriticalSection(&crit_section);
    RtlLeaveCriticalSection(&crit_section);
    test_passed &= assert_critical_section_equals(
        &crit_section,
        0,
        1,
        (HANDLE)KeGetCurrentThread(),
        "Leave critical section once"
    );

    RtlLeaveCriticalSection(&crit_section);
    test_passed &= assert_critical_section_equals(
        &crit_section,
        -1,
        0,
        NULL,
        "Leave critical section again"
    );

    RtlEnterCriticalSection(&crit_section);
    test_passed &= assert_critical_section_equals(
        &crit_section,
        0,
        1,
        (HANDLE)KeGetCurrentThread(),
        "Re-Enter Critical Section"
    );

    TEST_END();
}

TEST_FUNC(RtlLeaveCriticalSectionAndRegion)
{
    RTL_CRITICAL_SECTION crit_section;
    TEST_BEGIN();

    PKTHREAD thread = KeGetCurrentThread();
    const ULONG orig_APC_disable = thread->KernelApcDisable;

    RtlInitializeCriticalSection(&crit_section);
    RtlEnterCriticalSectionAndRegion(&crit_section);
    RtlEnterCriticalSectionAndRegion(&crit_section);

    RtlLeaveCriticalSectionAndRegion(&crit_section);
    test_passed &= assert_critical_section_equals(
        &crit_section,
        0,
        1,
        (HANDLE)thread,
        "Leave critical section and region after entering twice"
    );
    test_passed &= assert_critical_region(thread, orig_APC_disable - 2, "KernelApcDisable should be unchanged until critical section is freed");

    RtlLeaveCriticalSectionAndRegion(&crit_section);
    test_passed &= assert_critical_section_equals(
        &crit_section,
        -1,
        0,
        NULL,
        "Leave critical section and region a second time"
    );
    test_passed &= assert_critical_region(thread, orig_APC_disable - 1, "KernelApcDisable should now decrement as the critial section is freed");

    // Cleanup machine state
    KeLeaveCriticalRegion();

    TEST_END();
}

TEST_FUNC(RtlTryEnterCriticalSection)
{
    /* FIXME: This is a stub! implement this function! */
}
