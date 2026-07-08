#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtQueryMutant)
{
    TEST_BEGIN();

    // --- Query unowned mutant ---
    HANDLE handle = NULL;
    NTSTATUS status = NtCreateMutant(&handle, NULL, FALSE);
    GEN_CHECK(status, STATUS_SUCCESS, "create unowned");

    if (NT_SUCCESS(status)) {
        MUTANT_BASIC_INFORMATION info;
        status = NtQueryMutant(handle, &info);
        GEN_CHECK(status, STATUS_SUCCESS, "query unowned");
        GEN_CHECK(info.CurrentCount, 1, "unowned count=1");
        GEN_CHECK(info.OwnedByCaller, FALSE, "not owned");
        GEN_CHECK(info.AbandonedState, FALSE, "not abandoned");

        // Acquire it
        LARGE_INTEGER timeout;
        timeout.QuadPart = 0;
        NtWaitForSingleObject(handle, FALSE, &timeout);

        status = NtQueryMutant(handle, &info);
        GEN_CHECK(status, STATUS_SUCCESS, "query owned");
        GEN_CHECK(info.CurrentCount, 0, "owned count=0");
        GEN_CHECK(info.OwnedByCaller, TRUE, "owned by caller");

        NtReleaseMutant(handle, NULL);
        NtClose(handle);
    }

    // --- Invalid handle ---
    MUTANT_BASIC_INFORMATION info;
    status = NtQueryMutant((HANDLE)0xDEAD, &info);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle");

    TEST_END();
}
