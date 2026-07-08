#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtCreateMutant)
{
    TEST_BEGIN();

    // --- Create unowned mutant ---
    HANDLE handle = NULL;
    NTSTATUS status = NtCreateMutant(&handle, NULL, FALSE);
    GEN_CHECK(status, STATUS_SUCCESS, "create unowned");

    if (NT_SUCCESS(status)) {
        GEN_CHECK(handle != NULL, TRUE, "handle valid");

        MUTANT_BASIC_INFORMATION info;
        status = NtQueryMutant(handle, &info);
        GEN_CHECK(status, STATUS_SUCCESS, "query unowned");
        GEN_CHECK(info.CurrentCount, 1, "unowned count=1");
        GEN_CHECK(info.OwnedByCaller, FALSE, "not owned");
        GEN_CHECK(info.AbandonedState, FALSE, "not abandoned");

        NtClose(handle);
    }

    // --- Create initially-owned mutant ---
    handle = NULL;
    status = NtCreateMutant(&handle, NULL, TRUE);
    GEN_CHECK(status, STATUS_SUCCESS, "create owned");

    if (NT_SUCCESS(status)) {
        MUTANT_BASIC_INFORMATION info;
        status = NtQueryMutant(handle, &info);
        GEN_CHECK(status, STATUS_SUCCESS, "query owned");
        GEN_CHECK(info.CurrentCount, 0, "owned count=0");
        GEN_CHECK(info.OwnedByCaller, TRUE, "owned by caller");
        GEN_CHECK(info.AbandonedState, FALSE, "not abandoned");

        // Must release before close to avoid abandoned state
        NtReleaseMutant(handle, NULL);
        NtClose(handle);
    }

    TEST_END();
}
