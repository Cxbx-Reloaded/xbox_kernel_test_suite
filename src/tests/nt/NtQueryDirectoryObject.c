#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtQueryDirectoryObject)
{
    TEST_BEGIN();

    // Open root directory
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\");
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    HANDLE dir_handle = NULL;
    NTSTATUS status = NtOpenDirectoryObject(&dir_handle, &obj_attr);
    GEN_CHECK(status, STATUS_SUCCESS, "open root");

    if (NT_SUCCESS(status)) {
        // Query directory entries
        BYTE buffer[256];
        ULONG context = 0;
        ULONG return_length = 0;

        status = NtQueryDirectoryObject(
            dir_handle, buffer, sizeof(buffer),
            TRUE, // RestartScan
            &context, &return_length);

        // Root directory should have at least one entry (e.g., "Device")
        GEN_CHECK(status, STATUS_SUCCESS, "query first entry");
        GEN_CHECK(return_length > 0, TRUE, "got data");

        // Query next entry
        ULONG context2 = context;
        status = NtQueryDirectoryObject(
            dir_handle, buffer, sizeof(buffer),
            FALSE, // continue scan
            &context2, &return_length);
        // Should have more entries or STATUS_NO_MORE_ENTRIES
        BOOL valid = (status == STATUS_SUCCESS || status == STATUS_NO_MORE_ENTRIES);
        GEN_CHECK(valid, TRUE, "second query valid");

        NtClose(dir_handle);
    }

    // --- Invalid handle ---
    BYTE buf[64];
    ULONG ctx = 0, ret_len = 0;
    status = NtQueryDirectoryObject((HANDLE)0xDEAD, buf, sizeof(buf), TRUE, &ctx, &ret_len);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle");

    TEST_END();
}
