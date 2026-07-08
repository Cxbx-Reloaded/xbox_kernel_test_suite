#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtSetIoCompletion)
{
    TEST_BEGIN();

    HANDLE handle = NULL;
    NTSTATUS status = NtCreateIoCompletion(&handle, 0, NULL, 0);
    GEN_CHECK(status, STATUS_SUCCESS, "create iocp");

    if (NT_SUCCESS(status)) {
        // Post a completion packet
        status = NtSetIoCompletion(handle, (PVOID)0x1234, (PVOID)0x5678,
                                  STATUS_SUCCESS, 100);
        GEN_CHECK(status, STATUS_SUCCESS, "post packet 1");

        // Verify depth increased
        IO_COMPLETION_BASIC_INFORMATION info;
        NtQueryIoCompletion(handle, &info);
        GEN_CHECK(info.Depth, 1, "depth=1 after post");

        // Post another
        status = NtSetIoCompletion(handle, (PVOID)0xAAAA, (PVOID)0xBBBB,
                                  STATUS_CANCELLED, 200);
        GEN_CHECK(status, STATUS_SUCCESS, "post packet 2");

        NtQueryIoCompletion(handle, &info);
        GEN_CHECK(info.Depth, 2, "depth=2");

        // Drain them so we don't leak
        PVOID key, apc;
        IO_STATUS_BLOCK iosb;
        LARGE_INTEGER timeout;
        timeout.QuadPart = 0;
        NtRemoveIoCompletion(handle, &key, &apc, &iosb, &timeout);
        NtRemoveIoCompletion(handle, &key, &apc, &iosb, &timeout);

        NtClose(handle);
    }

    // --- Invalid handle ---
    status = NtSetIoCompletion((HANDLE)0xDEAD, NULL, NULL, STATUS_SUCCESS, 0);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle");

    TEST_END();
}
