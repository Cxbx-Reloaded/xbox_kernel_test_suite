#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/exception.h"
#include "assertions/defines.h"

TEST_FUNC(RtlRaiseException)
{
    TEST_BEGIN();

    // RtlRaiseException raises a software exception that can be caught by SEH
    NTSTATUS caught_code = STATUS_SUCCESS;
    EXCEPTION_RECORD record;
    RtlZeroMemory(&record, sizeof(record));
    record.ExceptionCode = 0xE0000001; // Custom exception code
    record.ExceptionFlags = 0;
    record.NumberParameters = 0;

    __try {
        RtlRaiseException(&record);
        // Should not reach here
        TEST_FAILED();
    }
    __except(GetExceptionStatus(&caught_code, EXCEPTION_EXECUTE_HANDLER)) {
    }
    GEN_CHECK(caught_code, 0xE0000001, "caught exception code");

    // Test with a different code
    caught_code = STATUS_SUCCESS;
    record.ExceptionCode = STATUS_INTEGER_OVERFLOW;
    __try {
        RtlRaiseException(&record);
        TEST_FAILED();
    }
    __except(GetExceptionStatus(&caught_code, EXCEPTION_EXECUTE_HANDLER)) {
    }
    GEN_CHECK(caught_code, STATUS_INTEGER_OVERFLOW, "caught INTEGER_OVERFLOW");

    TEST_END();
}

TEST_FUNC(RtlRaiseStatus)
{
    TEST_BEGIN();

    // RtlRaiseStatus raises a non-continuable exception with given status
    NTSTATUS caught_code = STATUS_SUCCESS;

    __try {
        RtlRaiseStatus(STATUS_UNSUCCESSFUL);
        // Should not reach here
        TEST_FAILED();
    }
    __except(GetExceptionStatus(&caught_code, EXCEPTION_EXECUTE_HANDLER)) {
    }
    GEN_CHECK(caught_code, STATUS_UNSUCCESSFUL, "caught STATUS_UNSUCCESSFUL");

    caught_code = STATUS_SUCCESS;
    __try {
        RtlRaiseStatus(STATUS_ACCESS_VIOLATION);
        TEST_FAILED();
    }
    __except(GetExceptionStatus(&caught_code, EXCEPTION_EXECUTE_HANDLER)) {
    }
    GEN_CHECK(caught_code, STATUS_ACCESS_VIOLATION, "caught ACCESS_VIOLATION");

    TEST_END();
}
