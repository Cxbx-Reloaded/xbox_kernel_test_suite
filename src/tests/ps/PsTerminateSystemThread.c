#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

static volatile ULONG term_exit_code_test = 0;

static VOID NTAPI terminate_success_thread(PVOID context)
{
    (void)context;
    PsTerminateSystemThread(STATUS_SUCCESS);
}

static VOID NTAPI terminate_custom_code_thread(PVOID context)
{
    (void)context;
    PsTerminateSystemThread((NTSTATUS)0xC000DEAD);
}

static VOID NTAPI terminate_after_work_thread(PVOID context)
{
    // Do some work, then terminate
    volatile ULONG* flag = (volatile ULONG*)context;
    *flag = 1;
    PsTerminateSystemThread(STATUS_SUCCESS);
    // Code after PsTerminateSystemThread should never execute
    *flag = 99;
}

TEST_FUNC(PsTerminateSystemThread)
{
    TEST_BEGIN();

    // --- Basic thread termination with STATUS_SUCCESS ---
    HANDLE thread_handle = NULL;
    NTSTATUS status = PsCreateSystemThread(
        &thread_handle, NULL,
        (PKSTART_ROUTINE)terminate_success_thread, NULL, FALSE
    );
    GEN_CHECK(status, STATUS_SUCCESS, "created success thread");
    if (NT_SUCCESS(status)) {
        status = NtWaitForSingleObject(thread_handle, FALSE, NULL);
        GEN_CHECK(status, STATUS_SUCCESS, "wait for terminated thread");
        NtClose(thread_handle);
    }

    // --- Thread with custom exit code ---
    status = PsCreateSystemThread(
        &thread_handle, NULL,
        (PKSTART_ROUTINE)terminate_custom_code_thread, NULL, FALSE
    );
    if (NT_SUCCESS(status)) {
        status = NtWaitForSingleObject(thread_handle, FALSE, NULL);
        GEN_CHECK(status, STATUS_SUCCESS, "custom code thread done");
        NtClose(thread_handle);
    }

    // --- Code after PsTerminateSystemThread never executes ---
    volatile ULONG flag = 0;
    status = PsCreateSystemThread(
        &thread_handle, NULL,
        (PKSTART_ROUTINE)terminate_after_work_thread,
        (PVOID)&flag, FALSE
    );
    if (NT_SUCCESS(status)) {
        NtWaitForSingleObject(thread_handle, FALSE, NULL);
        GEN_CHECK(flag, 1, "work done before terminate");
        // If the code after PsTerminateSystemThread ran, flag would be 99
        GEN_CHECK(flag != 99, TRUE, "post-terminate code did not run");
        NtClose(thread_handle);
    }

    // --- Multiple threads terminating concurrently ---
    HANDLE handles[4];
    ULONG i;
    for (i = 0; i < 4; i++) {
        PsCreateSystemThread(&handles[i], NULL,
            (PKSTART_ROUTINE)terminate_success_thread, NULL, FALSE);
    }
    for (i = 0; i < 4; i++) {
        if (handles[i]) {
            NtWaitForSingleObject(handles[i], FALSE, NULL);
            NtClose(handles[i]);
        }
    }
    GEN_CHECK(TRUE, TRUE, "4 concurrent terminations");

    TEST_END();
}
