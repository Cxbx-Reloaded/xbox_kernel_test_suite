#include <xboxkrnl/xboxkrnl.h>
#include <synchapi.h>
#include <fileapi.h>
#include <handleapi.h>

#include "util/output.h"
#include "assertions/object_type.h"
#include "util/device_dummy.h"

TEST_FUNC(IoCompletionObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&IoCompletionObjectType, 'pmoC', FALSE, TRUE, FALSE);
    HANDLE completion_handle;
    NTSTATUS status = NtCreateIoCompletion(&completion_handle, STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x3, NULL, 0);
    GEN_CHECK(completion_handle != INVALID_HANDLE_VALUE, TRUE, "completion_handle");
    if (completion_handle != INVALID_HANDLE_VALUE) {
        test_passed &= assert_object_header_type(&IoCompletionObjectType, completion_handle);
        PVOID key_context = NULL, apc_context = NULL;
        IO_STATUS_BLOCK status_block = { 0 };
        status = NtSetIoCompletion(completion_handle, &key_context, &apc_context, STATUS_SUCCESS, status_block.Information);
        GEN_CHECK(status, STATUS_SUCCESS, "status return");
        LARGE_INTEGER timeout = { .QuadPart = 0 };
        status = NtRemoveIoCompletion(completion_handle, &key_context, &apc_context, &status_block, &timeout);
        GEN_CHECK(status, STATUS_SUCCESS, "status return");
        GEN_CHECK(status_block.Status, STATUS_SUCCESS, "status block");
    }

    TEST_END();
}

TEST_FUNC(IoDeviceObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&IoDeviceObjectType, 'iveD', FALSE, FALSE, TRUE);
    ANSI_STRING obj_name;
    RtlInitAnsiString(&obj_name, "\\Device\\IoDeviceObjectType");
    NTSTATUS status = IoCreateDevice(&dummy_driver_object, 0, &obj_name, FILE_DEVICE_CD_ROM, FALSE, &dummy_device_object);
    GEN_CHECK(status, STATUS_SUCCESS, "status return");
    if (NT_SUCCESS(status)) {
        OBJECT_ATTRIBUTES obj_attr;
        InitializeObjectAttributes(&obj_attr, &obj_name, OBJ_OPENIF, NULL, NULL);
        HANDLE device_handle;
        status = ObOpenObjectByName(&obj_attr, NULL, NULL, &device_handle);
        GEN_CHECK(status, STATUS_SUCCESS, "status return");
        GEN_CHECK(device_handle != INVALID_HANDLE_VALUE, TRUE, "device_handle");
        if (device_handle != INVALID_HANDLE_VALUE) {
            test_passed &= assert_object_header_type(&IoDeviceObjectType, device_handle);
            BOOL close = CloseHandle(device_handle);
            GEN_CHECK(close, TRUE, "close handle");
        }
        IoDeleteDevice(dummy_device_object);
        dummy_device_object = NULL;
    }

    TEST_END();
}

TEST_FUNC(IoFileObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&IoFileObjectType, 'eliF', TRUE, TRUE, TRUE);
    HANDLE file_handle = CreateFileA("D:\\IoFileObjectType",
                                     GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                     NULL,
                                     CREATE_ALWAYS,
                                     FILE_ATTRIBUTE_NORMAL,
                                     NULL);
    GEN_CHECK(file_handle != INVALID_HANDLE_VALUE, TRUE, "file_handle");
    if (file_handle != INVALID_HANDLE_VALUE) {
        test_passed &= assert_object_header_type(&IoFileObjectType, file_handle);
        BOOL close = CloseHandle(file_handle);
        GEN_CHECK(close, TRUE, "close handle");
        BOOL delete = DeleteFileA("D:\\IoFileObjectType");
        GEN_CHECK(delete, TRUE, "delete file");
    }

    TEST_END();
}
