#include <xboxkrnl/xboxkrnl.h>
#include <windows.h>

#include "output.h"
#include "common_assertions.h"

void test_IoAllocateIrp(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoBuildAsynchronousFsdRequest(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoBuildDeviceIoControlRequest(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoBuildSynchronousFsdRequest(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoCheckShareAccess(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoCompletionObjectType(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoCreateDevice(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoCreateFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoCreateSymbolicLink(){
    const char* func_num = "0x0043";
    const char* func_name = "IoCreateSymbolicLink";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    ANSI_STRING symlink_str, symlink_bad_str, device_str, device_w_slash_str, device_bad_path_nonexistent_str;
    RtlInitAnsiString(&symlink_str, "\\xkts");
    RtlInitAnsiString(&symlink_bad_str, "\\xkts\\bad_symlink");
    RtlInitAnsiString(&device_str, "\\Device\\Harddisk0\\Partition5");
    RtlInitAnsiString(&device_w_slash_str, "\\Device\\Harddisk0\\Partition5\\");
    RtlInitAnsiString(&device_bad_path_nonexistent_str, "\\Device\\Harddisk0\\Partition5\\xkts_nonexistent");
    NTSTATUS result;

    result = IoCreateSymbolicLink(&symlink_str, &device_str);
    tests_passed &= assert_NTSTATUS(result, STATUS_SUCCESS, func_name);

    // Do not delete symbolic link yet in order to trigger object name collision error.
    result = IoCreateSymbolicLink(&symlink_str, &device_str);
    tests_passed &= assert_NTSTATUS(result, STATUS_OBJECT_NAME_COLLISION, func_name);

    IoDeleteSymbolicLink(&symlink_str);

    result = IoCreateSymbolicLink(&symlink_str, &device_w_slash_str);
    tests_passed &= assert_NTSTATUS(result, STATUS_SUCCESS, func_name);

    IoDeleteSymbolicLink(&symlink_str);

    result = IoCreateSymbolicLink(&symlink_str, &device_bad_path_nonexistent_str);
    tests_passed &= assert_NTSTATUS(result, STATUS_OBJECT_NAME_NOT_FOUND, func_name);

    result = IoCreateSymbolicLink(&symlink_bad_str, &device_str);
    tests_passed &= assert_NTSTATUS(result, STATUS_OBJECT_PATH_NOT_FOUND, func_name);

    result = IoCreateSymbolicLink(&device_bad_path_nonexistent_str, &device_str);
    tests_passed &= assert_NTSTATUS(result, STATUS_OBJECT_PATH_NOT_FOUND, func_name);

    print_test_footer(func_num, func_name, tests_passed);
}

void test_IoDeleteDevice(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoDeleteSymbolicLink(){
    const char* func_num = "0x0045";
    const char* func_name = "IoDeleteSymbolicLink";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    ANSI_STRING symlink_str, device_str;
    RtlInitAnsiString(&symlink_str, "\\xkts");
    RtlInitAnsiString(&device_str, "\\Device\\Harddisk0\\Partition5");
    NTSTATUS result;

    IoCreateSymbolicLink(&symlink_str, &device_str);

    result = IoDeleteSymbolicLink(&symlink_str);
    tests_passed &= assert_NTSTATUS(result, STATUS_SUCCESS, func_name);

    result = IoDeleteSymbolicLink(&symlink_str);
    tests_passed &= assert_NTSTATUS(result, STATUS_OBJECT_NAME_NOT_FOUND, func_name);

    print_test_footer(func_num, func_name, tests_passed);
}

void test_IoDeviceObjectType(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoFileObjectType(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoFreeIrp(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoInitializeIrp(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoInvalidDeviceRequest(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoQueryFileInformation(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoQueryVolumeInformation(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoQueueThreadIrp(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoRemoveShareAccess(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoSetIoCompletion(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoSetShareAccess(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoStartNextPacket(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoStartNextPacketByKey(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoStartPacket(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoSynchronousDeviceIoControlRequest(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoSynchronousFsdRequest(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoDismountVolume(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoDismountVolumeByName(){
    /* FIXME: This is a stub! implement this function! */
}

void test_IoMarkIrpMustComplete(){
    /* FIXME: This is a stub! implement this function! */
}
