#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtOpenDirectoryObject)
{
    TEST_BEGIN();

    // Open the root directory "\" which always exists
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\");
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    HANDLE dir_handle = NULL;
    NTSTATUS status = NtOpenDirectoryObject(&dir_handle, &obj_attr);
    GEN_CHECK(status, STATUS_SUCCESS, "open root dir");

    if (NT_SUCCESS(status)) {
        GEN_CHECK(dir_handle != NULL, TRUE, "handle valid");
        NtClose(dir_handle);
    }

    // Open "\Device" directory
    RtlInitAnsiString(&name, "\\Device");
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    dir_handle = NULL;
    status = NtOpenDirectoryObject(&dir_handle, &obj_attr);
    GEN_CHECK(status, STATUS_SUCCESS, "open Device dir");

    if (NT_SUCCESS(status)) {
        NtClose(dir_handle);
    }

    // Non-existent directory
    RtlInitAnsiString(&name, "\\NonExistentDir12345");
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);
    dir_handle = NULL;
    status = NtOpenDirectoryObject(&dir_handle, &obj_attr);
    GEN_CHECK(NT_SUCCESS(status), FALSE, "non-existent fails");

    TEST_END();
}
