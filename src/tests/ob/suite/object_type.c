#include <xboxkrnl/xboxkrnl.h>
#include <nxdk/mount.h>
#include <handleapi.h>

#include "util/output.h"
#include "assertions/object_type.h"

TEST_FUNC(ObDirectoryObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&ObDirectoryObjectType, 'eriD', FALSE, FALSE, FALSE);
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, NULL, OBJ_CASE_INSENSITIVE, ObDosDevicesDirectory(), NULL);
    HANDLE directory_handle;
    NTSTATUS status = NtOpenDirectoryObject(&directory_handle, &obj_attr);
    if (NT_SUCCESS(status)) {
        test_passed &= assert_object_header_type(&ObDirectoryObjectType, directory_handle);
        BOOL close = CloseHandle(directory_handle);
        GEN_CHECK(close, TRUE, "close handle");
    }

    TEST_END();
}

TEST_FUNC(ObSymbolicLinkObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&ObSymbolicLinkObjectType, 'bmyS', FALSE, TRUE, FALSE);
    ANSI_STRING obj_name;
    RtlInitAnsiString(&obj_name, "\\??\\A:");
    bool is_mount = nxMountDrive(obj_name.Buffer[4], "\\Device\\CdRom0");
    GEN_CHECK(is_mount, TRUE, "is_mount");
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &obj_name, OBJ_CASE_INSENSITIVE, NULL, NULL);
    HANDLE symlink_handle;
    NTSTATUS status = NtOpenSymbolicLinkObject(&symlink_handle, &obj_attr);
    if (NT_SUCCESS(status)) {
        test_passed &= assert_object_header_type(&ObSymbolicLinkObjectType, symlink_handle);
        BOOL close = CloseHandle(symlink_handle);
        GEN_CHECK(close, TRUE, "close handle");
    }
    if (is_mount) {
        nxUnmountDrive(obj_name.Buffer[4]);
    }

    TEST_END();
}
