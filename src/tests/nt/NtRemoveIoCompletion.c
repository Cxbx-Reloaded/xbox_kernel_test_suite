#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtRemoveIoCompletion)
{
    TEST_BEGIN();

    HANDLE handle = NULL;
    NTSTATUS status = NtCreateIoCompletion(&handle, 0, NULL, 0);
    GEN_CHECK(status, STATUS_SUCCESS, "create iocp");

    if (NT_SUCCESS(status)) {
        // Post known data
        NtSetIoCompletion(handle, (PVOID)0x1111, (PVOID)0x2222, STATUS_SUCCESS, 42);
        NtSetIoCompletion(handle, (PVOID)0x3333, (PVOID)0x4444, STATUS_CANCELLED, 99);

        // Remove first - should be FIFO
        PVOID key = NULL, apc = NULL;
        IO_STATUS_BLOCK iosb;
        LARGE_INTEGER timeout;
        timeout.QuadPart = 0;

        status = NtRemoveIoCompletion(handle, &key, &apc, &iosb, &timeout);
        GEN_CHECK(status, STATUS_SUCCESS, "remove first");
        GEN_CHECK(key, (PVOID)0x1111, "key1");
        GEN_CHECK(apc, (PVOID)0x2222, "apc1");
        GEN_CHECK(iosb.Status, STATUS_SUCCESS, "status1");
        GEN_CHECK((ULONG_PTR)iosb.Information, 42, "info1");

        // Remove second
        status = NtRemoveIoCompletion(handle, &key, &apc, &iosb, &timeout);
        GEN_CHECK(status, STATUS_SUCCESS, "remove second");
        GEN_CHECK(key, (PVOID)0x3333, "key2");
        GEN_CHECK(apc, (PVOID)0x4444, "apc2");
        GEN_CHECK(iosb.Status, STATUS_CANCELLED, "status2");
        GEN_CHECK((ULONG_PTR)iosb.Information, 99, "info2");

        // Remove from empty with timeout=0 should return STATUS_TIMEOUT
        status = NtRemoveIoCompletion(handle, &key, &apc, &iosb, &timeout);
        GEN_CHECK(status, STATUS_TIMEOUT, "empty returns timeout");

        // Verify depth is 0
        IO_COMPLETION_BASIC_INFORMATION info;
        NtQueryIoCompletion(handle, &info);
        GEN_CHECK(info.Depth, 0, "depth=0 after drain");

        NtClose(handle);
    }

    TEST_END();
}
