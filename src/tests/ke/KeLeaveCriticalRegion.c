#include <xboxkrnl/xboxkrnl.h>

#include "assertions/ke.h"
#include "util/output.h"

TEST_FUNC(KeLeaveCriticalRegion)
{
    TEST_BEGIN();

    PKTHREAD thread = KeGetCurrentThread();
    KeEnterCriticalRegion();
    KeEnterCriticalRegion();

    KeLeaveCriticalRegion();
    assert_critical_region(thread, -1, "Leave critical region after entering twice");

    KeLeaveCriticalRegion();
    assert_critical_region(thread, 0, "Leave critical region again");

    TEST_END();
}
