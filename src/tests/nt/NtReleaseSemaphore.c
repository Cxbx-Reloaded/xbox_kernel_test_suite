#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtReleaseSemaphore)
{
    TEST_BEGIN();

    HANDLE handle = NULL;
    NTSTATUS status = NtCreateSemaphore(&handle, NULL, 0, 5);
    GEN_CHECK(status, STATUS_SUCCESS, "create sem(0,5)");

    if (NT_SUCCESS(status)) {
        // Release 1 - previous count was 0
        LONG prev_count = -1;
        status = NtReleaseSemaphore(handle, 1, &prev_count);
        GEN_CHECK(status, STATUS_SUCCESS, "release 1");
        GEN_CHECK(prev_count, 0, "prev was 0");

        // Release 2 more - previous count was 1
        prev_count = -1;
        status = NtReleaseSemaphore(handle, 2, &prev_count);
        GEN_CHECK(status, STATUS_SUCCESS, "release 2");
        GEN_CHECK(prev_count, 1, "prev was 1");

        // Current count should be 3
        SEMAPHORE_BASIC_INFORMATION info;
        NtQuerySemaphore(handle, &info);
        GEN_CHECK(info.CurrentCount, 3, "count is 3");

        // Release that would exceed max should fail
        status = NtReleaseSemaphore(handle, 3, &prev_count);
        GEN_CHECK(status, STATUS_SEMAPHORE_LIMIT_EXCEEDED, "exceed max fails");

        // Count should be unchanged after failed release
        NtQuerySemaphore(handle, &info);
        GEN_CHECK(info.CurrentCount, 3, "count unchanged after failure");

        // NULL PreviousCount is allowed
        status = NtReleaseSemaphore(handle, 1, NULL);
        GEN_CHECK(status, STATUS_SUCCESS, "NULL prev OK");

        // Release with count 0 should fail
        status = NtReleaseSemaphore(handle, 0, NULL);
        GEN_CHECK(status, STATUS_INVALID_PARAMETER, "release 0 fails");

        NtClose(handle);
    }

    // --- Invalid handle ---
    LONG dummy;
    status = NtReleaseSemaphore((HANDLE)0xDEAD, 1, &dummy);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle");

    TEST_END();
}
