#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeSetEventBoostPriority)
{
    TEST_BEGIN();

    KEVENT event;
    KeInitializeEvent(&event, SynchronizationEvent, FALSE);

    // KeSetEventBoostPriority sets event with priority boost for waiter
    // With no waiters, it just sets the event
    KeSetEventBoostPriority(&event, NULL);
    GEN_CHECK(event.Header.SignalState, 1, "event set");

    TEST_END();
}
