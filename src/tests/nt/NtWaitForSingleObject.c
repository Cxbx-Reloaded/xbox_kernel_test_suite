#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtWaitForSingleObject)
{
    TEST_BEGIN();

    // --- Wait on signaled event (immediate satisfaction) ---
    HANDLE event = NULL;
    NTSTATUS status = NtCreateEvent(&event, NULL, NotificationEvent, TRUE);
    GEN_CHECK(status, STATUS_SUCCESS, "create signaled event");

    if (NT_SUCCESS(status)) {
        LARGE_INTEGER timeout;
        timeout.QuadPart = 0;
        status = NtWaitForSingleObject(event, FALSE, &timeout);
        GEN_CHECK(status, STATUS_SUCCESS, "signaled: immediate");

        // Reset event, wait with zero timeout should timeout
        NtClearEvent(event);
        status = NtWaitForSingleObject(event, FALSE, &timeout);
        GEN_CHECK(status, STATUS_TIMEOUT, "non-signaled: timeout");

        // Wait with NULL timeout would block forever - use short relative
        timeout.QuadPart = -10000LL; // 1ms
        status = NtWaitForSingleObject(event, FALSE, &timeout);
        GEN_CHECK(status, STATUS_TIMEOUT, "short timeout expires");

        NtClose(event);
    }

    // --- Wait on semaphore (auto-decrement) ---
    HANDLE sem = NULL;
    status = NtCreateSemaphore(&sem, NULL, 2, 5);
    if (NT_SUCCESS(status)) {
        LARGE_INTEGER timeout;
        timeout.QuadPart = 0;

        // Should succeed and decrement
        status = NtWaitForSingleObject(sem, FALSE, &timeout);
        GEN_CHECK(status, STATUS_SUCCESS, "sem wait 1");

        status = NtWaitForSingleObject(sem, FALSE, &timeout);
        GEN_CHECK(status, STATUS_SUCCESS, "sem wait 2");

        // Now count=0, should timeout
        status = NtWaitForSingleObject(sem, FALSE, &timeout);
        GEN_CHECK(status, STATUS_TIMEOUT, "sem exhausted");

        NtClose(sem);
    }

    // --- Invalid handle ---
    LARGE_INTEGER timeout;
    timeout.QuadPart = 0;
    status = NtWaitForSingleObject((HANDLE)0xDEAD, FALSE, &timeout);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle");

    TEST_END();
}
