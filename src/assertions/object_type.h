#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "defines.h"

static BOOL assert_object_type_ex(POBJECT_TYPE object_type,
                                  ULONG pool_tag,
                                  BOOL has_close,
                                  BOOL has_delete,
                                  BOOL has_parse,
                                  int func_line)
{
    ASSERT_HEADER;

    BOOLEAN is_address_valid;
    GEN_CHECK_EX(object_type->AllocateProcedure, ExAllocatePoolWithTag, ".AllocateProcedure", func_line);
    GEN_CHECK_EX(object_type->FreeProcedure, ExFreePool, ".FreeProcedure", func_line);
    // NOTE: Some object type has Close/Delete/Parse functions assigned.
    GEN_CHECK_EX(object_type->CloseProcedure != NULL, has_close, ".CloseProcedure", func_line);
    if (has_close) {
        is_address_valid = MmIsAddressValid(object_type->CloseProcedure);
        GEN_CHECK_EX(is_address_valid, TRUE, ".CloseProcedure valid", func_line);
    }
    GEN_CHECK_EX(object_type->DeleteProcedure != NULL, has_delete, ".DeleteProcedure", func_line);
    if (has_delete) {
        is_address_valid = MmIsAddressValid(object_type->DeleteProcedure);
        GEN_CHECK_EX(is_address_valid, TRUE, ".DeleteProcedure valid", func_line);
    }
    GEN_CHECK_EX(object_type->ParseProcedure != NULL, has_parse, ".ParseProcedure", func_line);
    if (has_parse) {
        is_address_valid = MmIsAddressValid(object_type->ParseProcedure);
        GEN_CHECK_EX(is_address_valid, TRUE, ".ParseProcedure valid", func_line);
    }
    // NOTE: DefaultObject check has been moved to assert_object_header_type_ex function. Because it needs the object's address.
    GEN_CHECK_EX(object_type->PoolTag, pool_tag, ".PoolTag", func_line);

    return test_passed;
}
#define assert_object_type(object_type, pool_tag, has_close, has_delete, has_parse) \
    assert_object_type_ex(object_type, pool_tag, has_close, has_delete, has_parse, __LINE__)

static BOOL assert_object_header_type_ex(POBJECT_TYPE object_type,
                                         HANDLE object_handle,
                                         int func_line)
{
    ASSERT_HEADER;

    PVOID type_object;
    NTSTATUS status = ObReferenceObjectByHandle(object_handle, object_type, &type_object);
    GEN_CHECK_EX(type_object != NULL, TRUE, "type_object", func_line);
    if (NT_SUCCESS(status)) {
        POBJECT_HEADER event_object_header = OBJECT_TO_OBJECT_HEADER(type_object);
        // NOTE: DefaultObject's relative offset or address (above 0x80000000) are private.
        //       Since they are private, we can only verify if the address is valid by call
        //       MmIsAddressValid function.
        BOOLEAN is_address_valid;
        if ((LONG_PTR)object_type->DefaultObject >= 0) {
            is_address_valid = MmIsAddressValid((PUCHAR)type_object + (LONG_PTR)object_type->DefaultObject);
        }
        else {
            is_address_valid = MmIsAddressValid(type_object);
        }
        GEN_CHECK_EX(is_address_valid, TRUE, ".DefaultObject valid", func_line);

        GEN_CHECK_EX(event_object_header->Type, object_type, "object_header->Type", func_line);
        ObfDereferenceObject(type_object);
    }

    return test_passed;
}
#define assert_object_header_type(object_type, object_handle) \
    assert_object_header_type_ex(object_type, object_handle, __LINE__)
