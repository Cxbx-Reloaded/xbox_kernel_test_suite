#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

static volatile ULONG sync_counter = 0;

static BOOLEAN NTAPI sync_routine(PVOID Context)
{
    sync_counter = *(PULONG)Context;
    return TRUE;
}

TEST_FUNC(KeSynchronizeExecution)
{
    TEST_BEGIN();

    // KeSynchronizeExecution requires a connected interrupt
    TEST_SKIP("requires connected interrupt");

    TEST_END();
}
