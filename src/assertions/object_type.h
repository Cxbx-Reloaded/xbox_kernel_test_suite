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
    // TODO: DefaultObject field require check against the object's address for some object types.
    GEN_CHECK_EX(object_type->PoolTag, pool_tag, ".PoolTag", func_line);

    return test_passed;
}
#define assert_object_type(object_type, pool_tag, has_close, has_delete, has_parse) \
    assert_object_type_ex(object_type, pool_tag, has_close, has_delete, has_parse, __LINE__)
