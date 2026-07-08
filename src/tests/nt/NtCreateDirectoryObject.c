#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtCreateDirectoryObject)
{
    TEST_BEGIN();

    // Create a named directory object
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\NtCreateDirObjTest");
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    HANDLE dir_handle = NULL;
    NTSTATUS status = NtCreateDirectoryObject(&dir_handle, &obj_attr);
    GEN_CHECK(status, STATUS_SUCCESS, "create directory object");

    if (NT_SUCCESS(status)) {
        GEN_CHECK(dir_handle != NULL, TRUE, "handle valid");

        // Can open the same directory
        HANDLE open_handle = NULL;
        status = NtOpenDirectoryObject(&open_handle, &obj_attr);
        GEN_CHECK(status, STATUS_SUCCESS, "open same dir");

        if (NT_SUCCESS(status)) {
            NtClose(open_handle);
        }

        NtClose(dir_handle);
    }

    TEST_END();
}
