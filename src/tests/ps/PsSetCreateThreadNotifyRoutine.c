#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

static volatile ULONG create_notify_count = 0;
static volatile ULONG destroy_notify_count = 0;
static volatile HANDLE last_created_id = NULL;

static VOID NTAPI thread_notify_routine(PETHREAD Thread, HANDLE ThreadId, BOOLEAN Create)
{
    (void)Thread;
    if (Create) {
        create_notify_count++;
        last_created_id = ThreadId;
    } else {
        destroy_notify_count++;
    }
}

static VOID NTAPI notify_test_thread(PVOID context)
{
    (void)context;
    PsTerminateSystemThread(STATUS_SUCCESS);
}

TEST_FUNC(PsSetCreateThreadNotifyRoutine)
{
    TEST_BEGIN();

    create_notify_count = 0;
    destroy_notify_count = 0;
    last_created_id = NULL;

    // --- Register notification ---
    NTSTATUS status = PsSetCreateThreadNotifyRoutine(thread_notify_routine);
    GEN_CHECK(status, STATUS_SUCCESS, "register notify");

    if (NT_SUCCESS(status)) {
        ULONG before_create = create_notify_count;
        ULONG before_destroy = destroy_notify_count;

        // --- Create thread: should trigger create notification ---
        HANDLE thread_handle = NULL;
        PsCreateSystemThread(&thread_handle, NULL,
            (PKSTART_ROUTINE)notify_test_thread, NULL, FALSE);
        if (thread_handle) {
            NtWaitForSingleObject(thread_handle, FALSE, NULL);
            NtClose(thread_handle);
        }

        GEN_CHECK(create_notify_count > before_create, TRUE, "create notify fired");
        GEN_CHECK(destroy_notify_count > before_destroy, TRUE, "destroy notify fired");
        GEN_CHECK(last_created_id != NULL, TRUE, "ThreadId non-NULL");

        // --- Create multiple threads: count increases ---
        ULONG before = create_notify_count;
        for (ULONG i = 0; i < 3; i++) {
            PsCreateSystemThread(&thread_handle, NULL,
                (PKSTART_ROUTINE)notify_test_thread, NULL, FALSE);
            if (thread_handle) {
                NtWaitForSingleObject(thread_handle, FALSE, NULL);
                NtClose(thread_handle);
            }
        }
        GEN_CHECK(create_notify_count >= before + 3, TRUE, "3 more creates notified");
    }

    // Note: On Xbox kernel, there is no PsRemoveCreateThreadNotifyRoutine.
    // The notification remains registered for the lifetime of the process.
    // This is acceptable for a test exe that runs once.

    TEST_END();
}
