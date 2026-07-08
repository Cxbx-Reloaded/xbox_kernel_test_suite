#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

static volatile ULONG wait_thread_signaled = 0;

static VOID NTAPI wait_signal_thread(PVOID context)
{
    PKEVENT event = (PKEVENT)context;
    // Small delay to ensure the main thread is blocked
    LARGE_INTEGER delay;
    delay.QuadPart = -50000; // 5ms
    KeDelayExecutionThread(KernelMode, FALSE, &delay);
    // Signal the event
    KeSetEvent(event, IO_NO_INCREMENT, FALSE);
    wait_thread_signaled = 1;
    PsTerminateSystemThread(STATUS_SUCCESS);
}

TEST_FUNC(KeWaitForSingleObject)
{
    TEST_BEGIN();

    // --- Already-signaled notification event: immediate return ---
    KEVENT event;
    KeInitializeEvent(&event, NotificationEvent, TRUE);
    NTSTATUS status = KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
    GEN_CHECK(status, STATUS_SUCCESS, "signaled notification event immediate");
    // Notification event stays signaled after wait
    GEN_CHECK(event.Header.SignalState, 1, "notification event still signaled");

    // --- Already-signaled synchronization event: immediate, auto-resets ---
    KEVENT sync_event;
    KeInitializeEvent(&sync_event, SynchronizationEvent, TRUE);
    status = KeWaitForSingleObject(&sync_event, Executive, KernelMode, FALSE, NULL);
    GEN_CHECK(status, STATUS_SUCCESS, "signaled sync event immediate");
    // Synchronization event auto-resets to unsignaled after satisfying wait
    GEN_CHECK(sync_event.Header.SignalState, 0, "sync event auto-reset");

    // --- Zero timeout (poll) on unsignaled event: returns STATUS_TIMEOUT ---
    KEVENT unsig_event;
    KeInitializeEvent(&unsig_event, NotificationEvent, FALSE);
    LARGE_INTEGER timeout;
    timeout.QuadPart = 0;
    status = KeWaitForSingleObject(&unsig_event, Executive, KernelMode, FALSE, &timeout);
    GEN_CHECK(status, STATUS_TIMEOUT, "zero timeout on unsignaled");

    // --- Short relative timeout on unsignaled event ---
    timeout.QuadPart = -10000; // 1ms relative
    LARGE_INTEGER before, after;
    KeQuerySystemTime(&before);
    status = KeWaitForSingleObject(&unsig_event, Executive, KernelMode, FALSE, &timeout);
    KeQuerySystemTime(&after);
    GEN_CHECK(status, STATUS_TIMEOUT, "1ms timeout on unsignaled");
    // Verify that some time actually elapsed (at least ~0.5ms in 100ns units)
    LONGLONG elapsed = after.QuadPart - before.QuadPart;
    GEN_CHECK(elapsed >= 5000, TRUE, "elapsed >= 0.5ms");

    // --- Wait satisfied by another thread ---
    KEVENT thread_event;
    KeInitializeEvent(&thread_event, NotificationEvent, FALSE);
    wait_thread_signaled = 0;

    HANDLE thread_handle = NULL;
    NTSTATUS thr_status = PsCreateSystemThread(
        &thread_handle, NULL,
        (PKSTART_ROUTINE)wait_signal_thread,
        (PVOID)&thread_event, FALSE
    );

    if (NT_SUCCESS(thr_status)) {
        // Wait indefinitely - thread will signal within ~5ms
        status = KeWaitForSingleObject(&thread_event, Executive, KernelMode, FALSE, NULL);
        GEN_CHECK(status, STATUS_SUCCESS, "wait satisfied by thread");
        GEN_CHECK(wait_thread_signaled, 1, "signaling thread ran");

        NtWaitForSingleObject(thread_handle, FALSE, NULL);
        NtClose(thread_handle);
    }

    // --- Wait on semaphore: decrements count ---
    KSEMAPHORE sem;
    KeInitializeSemaphore(&sem, 2, 10);
    status = KeWaitForSingleObject(&sem, Executive, KernelMode, FALSE, NULL);
    GEN_CHECK(status, STATUS_SUCCESS, "semaphore wait (count 2->1)");
    GEN_CHECK(sem.Header.SignalState, 1, "semaphore count decremented");
    status = KeWaitForSingleObject(&sem, Executive, KernelMode, FALSE, NULL);
    GEN_CHECK(status, STATUS_SUCCESS, "semaphore wait (count 1->0)");
    GEN_CHECK(sem.Header.SignalState, 0, "semaphore count at 0");
    // Now a poll should timeout
    timeout.QuadPart = 0;
    status = KeWaitForSingleObject(&sem, Executive, KernelMode, FALSE, &timeout);
    GEN_CHECK(status, STATUS_TIMEOUT, "semaphore exhausted, timeout");

    // --- Wait on mutant ---
    KMUTANT mutant;
    KeInitializeMutant(&mutant, FALSE); // not owned
    status = KeWaitForSingleObject(&mutant, Executive, KernelMode, FALSE, NULL);
    GEN_CHECK(status, STATUS_SUCCESS, "mutant acquired");
    GEN_CHECK(mutant.Header.SignalState, 0, "mutant now owned (not signaled)");
    KeReleaseMutant(&mutant, IO_NO_INCREMENT, FALSE, FALSE);

    // --- NULL timeout means infinite wait: tested above with thread ---

    // --- Alertable parameter ---
    // SKIP: Testing Alertable=TRUE requires queuing a user APC from another
    // thread and verifying STATUS_ALERTED or STATUS_USER_APC return. This is
    // complex and rarely used by Xbox titles. Skipped for stability.

    TEST_END();
}
