#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

static volatile ULONG threadex_ran = 0;

static VOID NTAPI test_threadex_proc(PKSTART_ROUTINE StartRoutine, PVOID StartContext)
{
    threadex_ran = 1;
    (void)StartRoutine;
    (void)StartContext;
    PsTerminateSystemThread(STATUS_SUCCESS);
}

TEST_FUNC(PsCreateSystemThreadEx)
{
    TEST_BEGIN();

    HANDLE thread_handle = NULL;
    threadex_ran = 0;

    NTSTATUS status = PsCreateSystemThreadEx(
        &thread_handle,
        0,     // ThreadExtraSize
        0,     // KernelStackSize (0 = default)
        0,     // TlsDataSize
        NULL,  // ThreadId
        NULL,  // StartContext1
        NULL,  // StartContext2
        FALSE, // CreateSuspended
        FALSE, // DebugStack
        (PKSYSTEM_ROUTINE)test_threadex_proc
    );
    GEN_CHECK(status, STATUS_SUCCESS, "thread created");

    if (NT_SUCCESS(status)) {
        NtWaitForSingleObject(thread_handle, FALSE, NULL);
        GEN_CHECK(threadex_ran, 1, "thread executed");
        NtClose(thread_handle);
    }

    TEST_END();
}
