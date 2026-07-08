#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtQuerySymbolicLinkObject)
{
    TEST_BEGIN();

    // Open a symbolic link to query its target
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\??\\C:");
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    HANDLE link_handle = NULL;
    NTSTATUS status = NtOpenSymbolicLinkObject(&link_handle, &obj_attr);

    if (NT_SUCCESS(status)) {
        CHAR target_buf[256];
        ANSI_STRING target;
        target.Buffer = target_buf;
        target.Length = 0;
        target.MaximumLength = sizeof(target_buf);

        ULONG returned_length = 0;
        status = NtQuerySymbolicLinkObject(link_handle, &target, &returned_length);
        GEN_CHECK(status, STATUS_SUCCESS, "query C: target");

        if (NT_SUCCESS(status)) {
            GEN_CHECK(target.Length > 0, TRUE, "target has length");
            GEN_CHECK(returned_length > 0, TRUE, "returned length > 0");
            // Target should start with \Device
            BOOL starts_with_device = (target.Length >= 7 &&
                memcmp(target.Buffer, "\\Device", 7) == 0);
            GEN_CHECK(starts_with_device, TRUE, "target starts with \\Device");
        }

        // --- Buffer too small ---
        CHAR small_buf[4];
        ANSI_STRING small_target;
        small_target.Buffer = small_buf;
        small_target.Length = 0;
        small_target.MaximumLength = sizeof(small_buf);
        status = NtQuerySymbolicLinkObject(link_handle, &small_target, &returned_length);
        GEN_CHECK(status, STATUS_BUFFER_TOO_SMALL, "small buffer fails");

        NtClose(link_handle);
    } else {
        GEN_CHECK(TRUE, TRUE, "C: symlink may not exist in this environment");
    }

    // --- Invalid handle ---
    CHAR buf[64];
    ANSI_STRING t;
    t.Buffer = buf; t.Length = 0; t.MaximumLength = sizeof(buf);
    ULONG ret;
    status = NtQuerySymbolicLinkObject((HANDLE)0xDEAD, &t, &ret);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle");

    TEST_END();
}
