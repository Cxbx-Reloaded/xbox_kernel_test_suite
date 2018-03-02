#include <hal/fileio.h>
#include "output.h"

void test_NtAllocateVirtualMemory(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtCancelTimer(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtClearEvent(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtClose(){
    const char* func_num = "0x00BB";
    const char* func_name = "NtClose";
    HANDLE handle;
    NTSTATUS status;
    OBJECT_ATTRIBUTES obj;
    IO_STATUS_BLOCK isb;
    ANSI_STRING obj_name;
    char filepath[200];
    BOOL tests_passed = 0;

    print_test_header(func_num, func_name);

    XConvertDOSFilenameToXBOX("./default.xbe", filepath);
    RtlInitAnsiString(&obj_name, filepath);

    obj.RootDirectory = NULL;
    obj.Attributes = OBJ_CASE_INSENSITIVE;
    obj.ObjectName = &obj_name;

    status = NtCreateFile(
    &handle,
    GENERIC_READ,
    &obj,
    &isb,
    NULL,
    FILE_ATTRIBUTE_NORMAL,
    0,
    FILE_OPEN,
    FILE_SYNCHRONOUS_IO_NONALERT);

    if(NtClose(handle)==0)
        tests_passed += 1;

    print_test_footer(func_num, func_name, tests_passed);
}

void test_NtCreateDirectoryObject(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtCreateEvent(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtCreateFile(){
    const char* func_num = "0x00BE";
    const char* func_name = "NtCreateFile";
    HANDLE handle;
    NTSTATUS status;
    OBJECT_ATTRIBUTES obj;
    IO_STATUS_BLOCK isb;
    ANSI_STRING obj_name;
    char filepath[200];
    BOOL tests_passed = 1;

    print_test_header(func_num, func_name);

    XConvertDOSFilenameToXBOX("./default.xbe", filepath);
    RtlInitAnsiString(&obj_name, filepath);

    obj.RootDirectory = NULL;
    obj.Attributes = OBJ_CASE_INSENSITIVE;
    obj.ObjectName = &obj_name;

    status = NtCreateFile(
    &handle,
    GENERIC_READ,
    &obj,
    &isb,
    NULL,
    FILE_ATTRIBUTE_NORMAL,
    0,
    FILE_OPEN,
    FILE_SYNCHRONOUS_IO_NONALERT);

    tests_passed &= NT_SUCCESS(status);
    NtClose(handle);

    print_test_footer(func_num, func_name, tests_passed);
}

void test_NtCreateIoCompletion(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtCreateMutant(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtCreateSemaphore(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtCreateTimer(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtDeleteFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtDeviceIoControlFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtDuplicateObject(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtFlushBuffersFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtFreeVirtualMemory(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtFsControlFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtOpenDirectoryObject(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtOpenFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtOpenSymbolicLinkObject(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtProtectVirtualMemory(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtPulseEvent(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryDirectoryFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueueApcThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryDirectoryObject(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryEvent(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryFullAttributesFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryInformationFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryIoCompletion(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryMutant(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQuerySemaphore(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQuerySymbolicLinkObject(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryTimer(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryVirtualMemory(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryVolumeInformationFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtReadFile(){ // We will read the xbe magic number as a test
    const char* func_num = "0x00DB";
    const char* func_name = "NtReadFile";
    HANDLE handle;
    NTSTATUS status;
    OBJECT_ATTRIBUTES obj;
    IO_STATUS_BLOCK isb;
    ANSI_STRING obj_name;
    char filepath[200];
    ULONG uSize = 0x0004;
    char read[0x0004];
    BOOL tests_passed = 1;

    print_test_header(func_num, func_name);

    XConvertDOSFilenameToXBOX("./default.xbe", filepath);
    RtlInitAnsiString(&obj_name, filepath);

    obj.RootDirectory = NULL;
    obj.Attributes = OBJ_CASE_INSENSITIVE;
    obj.ObjectName = &obj_name;

    status = NtCreateFile(
    &handle,
    GENERIC_READ,
    &obj,
    &isb,
    NULL,
    FILE_ATTRIBUTE_NORMAL,
    0,
    FILE_OPEN,
    FILE_SYNCHRONOUS_IO_NONALERT);

    if (!NT_SUCCESS(status)) {
        NtClose(handle);
        tests_passed = 0;
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }

    status = NtReadFile(
    handle,
    NULL,
    NULL,
    NULL,
    &isb,
    read,
    uSize,
    NULL);

    if (status == STATUS_PENDING)
        status = NtWaitForSingleObject((void*)handle, FALSE, (void*)NULL);

    tests_passed &= NT_SUCCESS(status);
    NtClose(handle);

    print_test_footer(func_num, func_name, tests_passed);
}

void test_NtReadFileScatter(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtReleaseMutant(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtReleaseSemaphore(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtRemoveIoCompletion(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtResumeThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtSetEvent(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtSetInformationFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtSetIoCompletion(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtSetSystemTime(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtSetTimerEx(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtSignalAndWaitForSingleObjectEx(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtSuspendThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtUserIoApcDispatcher(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtWaitForSingleObject(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtWaitForSingleObjectEx(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtWaitForMultipleObjectsEx(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtWriteFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtWriteFileGather(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtYieldExecution(){
    /* FIXME: This is a stub! implement this function! */
}
