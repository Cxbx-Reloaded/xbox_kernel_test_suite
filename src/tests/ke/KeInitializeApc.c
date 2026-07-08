#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

static VOID NTAPI dummy_kernel_routine(
    PKAPC Apc,
    PKNORMAL_ROUTINE *NormalRoutine,
    PVOID *NormalContext,
    PVOID *SystemArgument1,
    PVOID *SystemArgument2)
{
    (void)Apc; (void)NormalRoutine; (void)NormalContext;
    (void)SystemArgument1; (void)SystemArgument2;
}

TEST_FUNC(KeInitializeApc)
{
    TEST_BEGIN();

    KAPC apc;
    PKTHREAD thread = KeGetCurrentThread();

    KeInitializeApc(
        &apc,
        thread,
        dummy_kernel_routine,
        NULL,  // RundownRoutine
        NULL,  // NormalRoutine
        KernelMode,
        NULL   // NormalContext
    );

    GEN_CHECK(apc.Thread, thread, "thread set");
    GEN_CHECK(apc.KernelRoutine == dummy_kernel_routine, TRUE, "routine set");

    TEST_END();
}
