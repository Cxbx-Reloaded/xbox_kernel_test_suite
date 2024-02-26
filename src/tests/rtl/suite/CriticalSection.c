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
    // Enter an unused critical section
    assert_critical_section_equals(&crit_section, 0, 1, (HANDLE)KeGetCurrentThread());
    // Enter the critical section again
    RtlEnterCriticalSection(&crit_section);
    assert_critical_section_equals(&crit_section, 1, 2, (HANDLE)KeGetCurrentThread());

    RtlLeaveCriticalSection(&crit_section);
    RtlLeaveCriticalSection(&crit_section);
    RtlEnterCriticalSection(&crit_section);
    // Re-Enter critical section after leaving twice
    assert_critical_section_equals(&crit_section, 0, 1, (HANDLE)KeGetCurrentThread());

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
    // Enter critcal section and region twice
    assert_critical_section_equals(&crit_section, 1, 2, (HANDLE)thread);
    assert_critical_region(thread, orig_APC_disable - 2);

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
    // Init critical section
    assert_critical_section_equals(&crit_section, -1, 0, NULL);

    memset(&crit_section, 0x11, sizeof(crit_section));
    RtlInitializeCriticalSection(&crit_section);
    // Re-Init critical section after setting garbage data
    assert_critical_section_equals(&crit_section, -1, 0, NULL);

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
    // Leave critical section once
    assert_critical_section_equals(&crit_section, 0, 1, (HANDLE)KeGetCurrentThread());
    // Leave critical section again
    RtlLeaveCriticalSection(&crit_section);
    assert_critical_section_equals(&crit_section, -1, 0, NULL);
    // Re-Enter Critical Section
    RtlEnterCriticalSection(&crit_section);
    assert_critical_section_equals(&crit_section, 0, 1, (HANDLE)KeGetCurrentThread());

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
    // Leave critical section and region after entering twice
    assert_critical_section_equals(&crit_section, 0, 1, (HANDLE)thread);
    // KernelApcDisable should be unchanged until critical section is freed
    assert_critical_region(thread, orig_APC_disable - 2);

    RtlLeaveCriticalSectionAndRegion(&crit_section);
    // Leave critical section and region a second time
    assert_critical_section_equals(&crit_section, -1, 0, NULL);
    // KernelApcDisable should now decrement as the critial section is freed
    assert_critical_region(thread, orig_APC_disable - 1);

    // Cleanup machine state
    KeLeaveCriticalRegion();

    TEST_END();
}

TEST_FUNC(RtlTryEnterCriticalSection)
{
    /* FIXME: This is a stub! implement this function! */
}
