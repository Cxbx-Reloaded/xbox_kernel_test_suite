#include <xboxkrnl/xboxkrnl.h>

#include "assertions/ke.h"
#include "util/output.h"

TEST_FUNC(KeEnterCriticalRegion)
{
    TEST_BEGIN();

    PKTHREAD thread = KeGetCurrentThread();
    // Before entering critical region
    assert_critical_region(thread, 0);

    KeEnterCriticalRegion();
    // In critical region
    assert_critical_region(thread, -1);

    KeEnterCriticalRegion();
    // Entered critical region again
    assert_critical_region(thread, -2);

    KeLeaveCriticalRegion();
    KeLeaveCriticalRegion();
    // Leave critical region twice
    assert_critical_region(thread, 0);

    TEST_END();
}
