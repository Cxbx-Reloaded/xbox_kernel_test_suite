#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtQueryIoCompletion)
{
    TEST_BEGIN();

    HANDLE handle = NULL;
    NTSTATUS status = NtCreateIoCompletion(&handle, 0, NULL, 0);
    GEN_CHECK(status, STATUS_SUCCESS, "create iocp");

    if (NT_SUCCESS(status)) {
        IO_COMPLETION_BASIC_INFORMATION info;

        // Empty queue
        status = NtQueryIoCompletion(handle, &info);
        GEN_CHECK(status, STATUS_SUCCESS, "query empty");
        GEN_CHECK(info.Depth, 0, "depth=0");

        // Post 3 items
        for (ULONG i = 0; i < 3; i++) {
            NtSetIoCompletion(handle, (PVOID)(ULONG_PTR)i, NULL, STATUS_SUCCESS, 0);
        }

        status = NtQueryIoCompletion(handle, &info);
        GEN_CHECK(status, STATUS_SUCCESS, "query with 3");
        GEN_CHECK(info.Depth, 3, "depth=3");

        // Remove one
        PVOID key, apc;
        IO_STATUS_BLOCK iosb;
        LARGE_INTEGER timeout;
        timeout.QuadPart = 0;
        NtRemoveIoCompletion(handle, &key, &apc, &iosb, &timeout);

        status = NtQueryIoCompletion(handle, &info);
        GEN_CHECK(status, STATUS_SUCCESS, "query after remove");
        GEN_CHECK(info.Depth, 2, "depth=2");

        // Clean up
        NtRemoveIoCompletion(handle, &key, &apc, &iosb, &timeout);
        NtRemoveIoCompletion(handle, &key, &apc, &iosb, &timeout);
        NtClose(handle);
    }

    // --- Invalid handle ---
    IO_COMPLETION_BASIC_INFORMATION info;
    status = NtQueryIoCompletion((HANDLE)0xDEAD, &info);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle");

    TEST_END();
}
