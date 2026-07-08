#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtCreateIoCompletion)
{
    TEST_BEGIN();

    // --- Create with default concurrency (0) ---
    HANDLE handle = NULL;
    NTSTATUS status = NtCreateIoCompletion(&handle, 0, NULL, 0);
    GEN_CHECK(status, STATUS_SUCCESS, "create default");

    if (NT_SUCCESS(status)) {
        GEN_CHECK(handle != NULL, TRUE, "handle valid");

        IO_COMPLETION_BASIC_INFORMATION info;
        status = NtQueryIoCompletion(handle, &info);
        GEN_CHECK(status, STATUS_SUCCESS, "query empty");
        GEN_CHECK(info.Depth, 0, "no pending items");

        NtClose(handle);
    }

    // --- Create with specified concurrency ---
    handle = NULL;
    status = NtCreateIoCompletion(&handle, 0, NULL, 4);
    GEN_CHECK(status, STATUS_SUCCESS, "create concurrency=4");

    if (NT_SUCCESS(status)) {
        NtClose(handle);
    }

    TEST_END();
}
