#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

static volatile ULONG thread_ran = 0;
static volatile ULONG thread_context_value = 0;

static VOID NTAPI test_thread_proc(PVOID context)
{
    thread_context_value = (ULONG)(ULONG_PTR)context;
    thread_ran = 1;
    PsTerminateSystemThread(STATUS_SUCCESS);
}

static VOID NTAPI test_thread_irql(PVOID context)
{
    // Thread should start at PASSIVE_LEVEL
    KIRQL irql = KeGetCurrentIrql();
    *(volatile KIRQL*)context = irql;
    PsTerminateSystemThread(STATUS_SUCCESS);
}

TEST_FUNC(PsCreateSystemThread)
{
    TEST_BEGIN();

    // --- Basic thread creation and execution ---
    HANDLE thread_handle = NULL;
    thread_ran = 0;
    thread_context_value = 0;

    NTSTATUS status = PsCreateSystemThread(
        &thread_handle,
        NULL,  // ThreadAttributes
        (PKSTART_ROUTINE)test_thread_proc,
        (PVOID)0x42,  // Context
        FALSE  // DebugStack
    );
    GEN_CHECK(status, STATUS_SUCCESS, "thread created");
    GEN_CHECK(thread_handle != NULL, TRUE, "handle not NULL");

    if (NT_SUCCESS(status)) {
        // Wait for thread to finish
        status = NtWaitForSingleObject(thread_handle, FALSE, NULL);
        GEN_CHECK(status, STATUS_SUCCESS, "wait for thread");
        GEN_CHECK(thread_ran, 1, "thread executed");
        GEN_CHECK(thread_context_value, 0x42, "context passed correctly");
        NtClose(thread_handle);
    }

    // --- Context value is properly forwarded (different values) ---
    thread_ran = 0;
    thread_context_value = 0;
    status = PsCreateSystemThread(
        &thread_handle, NULL,
        (PKSTART_ROUTINE)test_thread_proc,
        (PVOID)0xDEADBEEF, FALSE
    );
    if (NT_SUCCESS(status)) {
        NtWaitForSingleObject(thread_handle, FALSE, NULL);
        GEN_CHECK(thread_context_value, 0xDEADBEEF, "large context value");
        NtClose(thread_handle);
    }

    // --- NULL context ---
    thread_ran = 0;
    thread_context_value = 0xFFFF;
    status = PsCreateSystemThread(
        &thread_handle, NULL,
        (PKSTART_ROUTINE)test_thread_proc,
        NULL, FALSE
    );
    if (NT_SUCCESS(status)) {
        NtWaitForSingleObject(thread_handle, FALSE, NULL);
        GEN_CHECK(thread_ran, 1, "thread ran with NULL context");
        GEN_CHECK(thread_context_value, 0, "NULL context received");
        NtClose(thread_handle);
    }

    // --- Thread starts at PASSIVE_LEVEL ---
    volatile KIRQL thread_irql = 0xFF;
    status = PsCreateSystemThread(
        &thread_handle, NULL,
        (PKSTART_ROUTINE)test_thread_irql,
        (PVOID)&thread_irql, FALSE
    );
    if (NT_SUCCESS(status)) {
        NtWaitForSingleObject(thread_handle, FALSE, NULL);
        GEN_CHECK(thread_irql, PASSIVE_LEVEL, "thread starts at PASSIVE_LEVEL");
        NtClose(thread_handle);
    }

    // --- Handle is waitable (already tested above, but verify timeout works) ---
    status = PsCreateSystemThread(
        &thread_handle, NULL,
        (PKSTART_ROUTINE)test_thread_proc,
        (PVOID)1, FALSE
    );
    if (NT_SUCCESS(status)) {
        // Poll immediately - thread may or may not have finished
        LARGE_INTEGER zero_timeout;
        zero_timeout.QuadPart = 0;
        NTSTATUS poll = NtWaitForSingleObject(thread_handle, FALSE, &zero_timeout);
        // Either SUCCESS (already done) or TIMEOUT (still running) - both valid
        GEN_CHECK(poll == STATUS_SUCCESS || poll == STATUS_TIMEOUT, TRUE, "poll returns valid status");
        // Now wait for real
        NtWaitForSingleObject(thread_handle, FALSE, NULL);
        NtClose(thread_handle);
    }

    // --- NULL handle pointer ---
    // SKIP: Passing NULL for the handle pointer would write to address 0,
    // causing an access violation. This is undefined behavior that crashes.

    // --- NULL start routine ---
    // SKIP: A NULL start routine would cause the new thread to jump to address 0
    // when scheduled, causing an immediate access violation in the new thread
    // context. This typically bugchecks the system.

    TEST_END();
}
