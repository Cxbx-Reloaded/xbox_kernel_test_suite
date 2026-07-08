#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeSetTimerEx)
{
    TEST_BEGIN();

    KTIMER timer;
    KeInitializeTimerEx(&timer, NotificationTimer);

    // Set timer with no period (one-shot)
    LARGE_INTEGER due_time;
    due_time.QuadPart = -10000; // 1ms

    BOOLEAN was_set = KeSetTimerEx(&timer, due_time, 0, NULL);
    GEN_CHECK(was_set, FALSE, "not previously set");

    NTSTATUS status = KeWaitForSingleObject(&timer, Executive, KernelMode, FALSE, NULL);
    GEN_CHECK(status, STATUS_SUCCESS, "one-shot timer fired");

    TEST_END();
}
