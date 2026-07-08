#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtReleaseMutant)
{
    TEST_BEGIN();

    // Create initially-owned mutant
    HANDLE handle = NULL;
    NTSTATUS status = NtCreateMutant(&handle, NULL, TRUE);
    GEN_CHECK(status, STATUS_SUCCESS, "create owned mutant");

    if (NT_SUCCESS(status)) {
        // Release once - should return previous count (0 when owned once)
        LONG prev_count = -99;
        status = NtReleaseMutant(handle, &prev_count);
        GEN_CHECK(status, STATUS_SUCCESS, "release succeeds");
        GEN_CHECK(prev_count, 0, "prev count was 0 (owned)");

        // Mutant is now unowned (count=1), query to verify
        MUTANT_BASIC_INFORMATION info;
        NtQueryMutant(handle, &info);
        GEN_CHECK(info.CurrentCount, 1, "now unowned");
        GEN_CHECK(info.OwnedByCaller, FALSE, "not owned anymore");

        // Releasing an unowned mutant should fail
        status = NtReleaseMutant(handle, &prev_count);
        GEN_CHECK(status, STATUS_MUTANT_NOT_OWNED, "release unowned fails");

        // Re-acquire by waiting, then recursive acquire
        LARGE_INTEGER timeout;
        timeout.QuadPart = 0;
        status = NtWaitForSingleObject(handle, FALSE, &timeout);
        GEN_CHECK(status, STATUS_SUCCESS, "acquire by wait");

        // Recursively acquire again
        status = NtWaitForSingleObject(handle, FALSE, &timeout);
        GEN_CHECK(status, STATUS_SUCCESS, "recursive acquire");

        NtQueryMutant(handle, &info);
        GEN_CHECK(info.CurrentCount, -1, "recursive count=-1");

        // Must release twice to fully release
        NtReleaseMutant(handle, &prev_count);
        GEN_CHECK(prev_count, -1, "first release from -1");

        NtReleaseMutant(handle, &prev_count);
        GEN_CHECK(prev_count, 0, "second release from 0");

        // NULL PreviousCount is allowed
        NtWaitForSingleObject(handle, FALSE, &timeout);
        status = NtReleaseMutant(handle, NULL);
        GEN_CHECK(status, STATUS_SUCCESS, "NULL prev OK");

        NtClose(handle);
    }

    // --- Invalid handle ---
    LONG dummy;
    status = NtReleaseMutant((HANDLE)0xDEAD, &dummy);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle");

    TEST_END();
}
