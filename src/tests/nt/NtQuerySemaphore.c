#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtQuerySemaphore)
{
    TEST_BEGIN();

    HANDLE handle = NULL;
    NTSTATUS status = NtCreateSemaphore(&handle, NULL, 3, 10);
    GEN_CHECK(status, STATUS_SUCCESS, "create sem(3,10)");

    if (NT_SUCCESS(status)) {
        SEMAPHORE_BASIC_INFORMATION info;

        // Query initial state
        status = NtQuerySemaphore(handle, &info);
        GEN_CHECK(status, STATUS_SUCCESS, "query succeeds");
        GEN_CHECK(info.CurrentCount, 3, "initial count");
        GEN_CHECK(info.MaximumCount, 10, "max count");

        // Modify and re-query
        NtReleaseSemaphore(handle, 2, NULL);
        status = NtQuerySemaphore(handle, &info);
        GEN_CHECK(status, STATUS_SUCCESS, "query after release");
        GEN_CHECK(info.CurrentCount, 5, "count after release");
        GEN_CHECK(info.MaximumCount, 10, "max unchanged");

        // Wait to decrement and re-query
        LARGE_INTEGER timeout;
        timeout.QuadPart = 0; // immediate
        NtWaitForSingleObject(handle, FALSE, &timeout);
        status = NtQuerySemaphore(handle, &info);
        GEN_CHECK(status, STATUS_SUCCESS, "query after wait");
        GEN_CHECK(info.CurrentCount, 4, "count after wait");

        NtClose(handle);
    }

    // --- Invalid handle ---
    SEMAPHORE_BASIC_INFORMATION info;
    status = NtQuerySemaphore((HANDLE)0xDEAD, &info);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle");

    TEST_END();
}
