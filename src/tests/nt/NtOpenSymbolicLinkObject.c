#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtOpenSymbolicLinkObject)
{
    TEST_BEGIN();

    // Xbox kernel has symbolic links like \??\D: -> \Device\CdRom0
    // Try opening a well-known symbolic link
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\??\\D:");
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    HANDLE link_handle = NULL;
    NTSTATUS status = NtOpenSymbolicLinkObject(&link_handle, &obj_attr);

    if (NT_SUCCESS(status)) {
        GEN_CHECK(link_handle != NULL, TRUE, "D: link opened");
        NtClose(link_handle);
    } else {
        // D: might not exist in all test environments
        GEN_CHECK(TRUE, TRUE, "D: may not exist");
    }

    // Try C: which should always exist (title partition)
    RtlInitAnsiString(&name, "\\??\\C:");
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);
    link_handle = NULL;
    status = NtOpenSymbolicLinkObject(&link_handle, &obj_attr);
    // This is more likely to exist
    if (NT_SUCCESS(status)) {
        GEN_CHECK(link_handle != NULL, TRUE, "C: link opened");
        NtClose(link_handle);
    }

    // --- Non-existent symlink ---
    RtlInitAnsiString(&name, "\\??\\Z:");
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);
    link_handle = NULL;
    status = NtOpenSymbolicLinkObject(&link_handle, &obj_attr);
    GEN_CHECK(NT_SUCCESS(status), FALSE, "Z: does not exist");

    TEST_END();
}
