#include <xboxkrnl/xboxkrnl.h>
#include <stddef.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"
#include "assertions/common.h"

void test_ObReferenceObjectByHandle()
{
    const char* func_num = "0x00F6";
    const char* func_name = "ObReferenceObjectByHandle";
    BOOL test_passed = 1;
    PVOID object_return;
    NTSTATUS result;

    print_test_header(func_num, func_name);

    typedef struct {
        const char* message;
        HANDLE handle;
        POBJECT_TYPE object_type;
        NTSTATUS expected_result;
        PVOID expected_object_return;
    } reference_object_array;

    reference_object_array reference_object_handle_test[] = {
        // test with invalid handle with null object type
        { "unknown (invalid / NULL)", (HANDLE)100000, NULL, STATUS_INVALID_HANDLE, NULL },
        // test with invalid handle with null object type
        { "unknown (invalid / invalid)", (HANDLE)100000, (POBJECT_TYPE)&func_name, STATUS_INVALID_HANDLE, NULL },
        // test with invalid handle with thread object type
        { "thread (invalid / PsThreadObjectType)", (HANDLE)100000, &PsThreadObjectType, STATUS_INVALID_HANDLE, NULL },
        // test with invalid special handle (NtCurrentProcess) with null object type
        { "thread (-1 aka NtCurrentProcess / NULL)", (HANDLE)-1, NULL, STATUS_INVALID_HANDLE, NULL },
        // test with invalid special handle (NtCurrentProcess) with thread object type
        { "thread (-1 aka NtCurrentProcess / PsThreadObjectType)", (HANDLE)-1, &PsThreadObjectType, STATUS_INVALID_HANDLE, NULL },
        // test with valid special handle (NtCurrentThread) with null object type
        { "thread (-2 aka NtCurrentThread / NULL)", (HANDLE)-2, NULL, STATUS_SUCCESS, (PVOID)KeGetCurrentThread() },
        // test with valid special handle (NtCurrentThread) with thread object type
        { "thread (-2 aka NtCurrentThread / PsThreadObjectType)", (HANDLE)-2, &PsThreadObjectType, STATUS_SUCCESS, (PVOID)KeGetCurrentThread() },
        // test with valid special handle (NtCurrentThread) with symbolic link object type
        { "thread (-2 aka NtCurrentThread / ObSymbolicLinkObjectType)", (HANDLE)-2, &ObSymbolicLinkObjectType, STATUS_OBJECT_TYPE_MISMATCH, NULL },
    };
    size_t reference_object_handle_test_size = ARRAY_SIZE(reference_object_handle_test);

    for (unsigned i = 0; i < reference_object_handle_test_size; i++) {
        result = ObReferenceObjectByHandle(reference_object_handle_test[i].handle, reference_object_handle_test[i].object_type, &object_return);
        if (result == STATUS_SUCCESS) {
            NtClose(reference_object_handle_test[i].handle);
        }
        test_passed &= assert_NTSTATUS(result, reference_object_handle_test[i].expected_result, func_name);
        GEN_CHECK(object_return, reference_object_handle_test[i].expected_object_return, reference_object_handle_test[i].message);
    }

    print_test_footer(func_num, func_name, test_passed);
}
