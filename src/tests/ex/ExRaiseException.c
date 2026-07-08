#include <xboxkrnl/xboxkrnl.h>
#include <excpt.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ExRaiseException)
{
    TEST_BEGIN();

    // --- Basic custom exception code ---
    EXCEPTION_RECORD record;
    BOOLEAN caught = FALSE;
    NTSTATUS caught_code = 0;

    RtlZeroMemory(&record, sizeof(record));
    record.ExceptionCode = 0xE0000001; // Custom exception code
    record.ExceptionFlags = 0;
    record.NumberParameters = 0;

    __try {
        ExRaiseException(&record);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        caught = TRUE;
        caught_code = GetExceptionCode();
    }
    GEN_CHECK(caught, TRUE, "custom exception caught");
    GEN_CHECK(caught_code, 0xE0000001, "correct exception code");

    // --- Different exception code ---
    caught = FALSE;
    caught_code = 0;
    RtlZeroMemory(&record, sizeof(record));
    record.ExceptionCode = 0xE0000099;
    record.ExceptionFlags = 0;
    record.NumberParameters = 0;

    __try {
        ExRaiseException(&record);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        caught = TRUE;
        caught_code = GetExceptionCode();
    }
    GEN_CHECK(caught, TRUE, "second exception caught");
    GEN_CHECK(caught_code, 0xE0000099, "second code correct");

    // --- Exception with parameters ---
    caught = FALSE;
    RtlZeroMemory(&record, sizeof(record));
    record.ExceptionCode = 0xE0000002;
    record.ExceptionFlags = 0;
    record.NumberParameters = 2;
    record.ExceptionInformation[0] = 0xAAAA;
    record.ExceptionInformation[1] = 0xBBBB;

    __try {
        ExRaiseException(&record);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        caught = TRUE;
    }
    GEN_CHECK(caught, TRUE, "exception with params caught");

    // --- EXCEPTION_NONCONTINUABLE flag ---
    caught = FALSE;
    RtlZeroMemory(&record, sizeof(record));
    record.ExceptionCode = 0xE0000003;
    record.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
    record.NumberParameters = 0;

    __try {
        ExRaiseException(&record);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        caught = TRUE;
    }
    GEN_CHECK(caught, TRUE, "noncontinuable caught by SEH");

    // --- Multiple raises in sequence (re-entrant safety) ---
    ULONG raise_count = 0;
    for (ULONG i = 0; i < 5; i++) {
        RtlZeroMemory(&record, sizeof(record));
        record.ExceptionCode = 0xE0000010 + i;
        record.ExceptionFlags = 0;
        record.NumberParameters = 0;

        __try {
            ExRaiseException(&record);
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            raise_count++;
        }
    }
    GEN_CHECK(raise_count, 5, "5 sequential raises all caught");

    // --- ExRaiseException with existing ExceptionRecord chain ---
    // SKIP: Setting ExceptionRecord->ExceptionRecord (nested) requires careful
    // setup and the exception dispatcher may walk the chain. Testing chained
    // records risks stack overflow in the exception dispatcher if malformed.

    TEST_END();
}
