#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

static volatile ULONG apc_executed = 0;

static VOID NTAPI apc_kernel_routine(
    PKAPC Apc,
    PKNORMAL_ROUTINE *NormalRoutine,
    PVOID *NormalContext,
    PVOID *SystemArgument1,
    PVOID *SystemArgument2)
{
    (void)NormalRoutine; (void)NormalContext;
    (void)SystemArgument1; (void)SystemArgument2;
    apc_executed = 1;
    // Free the APC since it's been delivered
    (void)Apc;
}

TEST_FUNC(KeInsertQueueApc)
{
    TEST_BEGIN();

    // Queuing an APC to the current thread is tricky - it may fire immediately
    // when IRQL drops. Test carefully.
    KAPC apc;
    PKTHREAD thread = KeGetCurrentThread();

    apc_executed = 0;
    KeInitializeApc(&apc, thread, apc_kernel_routine, NULL, NULL, KernelMode, NULL);

    BOOLEAN inserted = KeInsertQueueApc(&apc, NULL, NULL, IO_NO_INCREMENT);
    GEN_CHECK(inserted, TRUE, "APC queued");

    // The APC should fire when we're at PASSIVE_LEVEL
    // Give it a chance to execute
    LARGE_INTEGER delay;
    delay.QuadPart = -10000; // 1ms
    KeDelayExecutionThread(KernelMode, FALSE, &delay);

    GEN_CHECK(apc_executed, 1, "APC executed");

    TEST_END();
}
