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
    // Leave critical region after entering twice
    assert_critical_region(thread, -1);

    KeLeaveCriticalRegion();
    // Leave critical region again
    assert_critical_region(thread, 0);

    TEST_END();
}
