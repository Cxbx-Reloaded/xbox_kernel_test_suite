#include <xboxkrnl/xboxkrnl.h>

#include "assertions/ke.h"
#include "util/output.h"

TEST_FUNC(KeEnterCriticalRegion)
{
    TEST_BEGIN();

    PKTHREAD thread = KeGetCurrentThread();
    test_passed &= assert_critical_region(thread, 0, "Before entering critical region");

    KeEnterCriticalRegion();
    test_passed &= assert_critical_region(thread, -1, "In critical region");

    KeEnterCriticalRegion();
    test_passed &= assert_critical_region(thread, -2, "Entered critical region again");

    KeLeaveCriticalRegion();
    KeLeaveCriticalRegion();
    test_passed &= assert_critical_region(thread, 0, "Leave critical region twice");

    TEST_END();
}
