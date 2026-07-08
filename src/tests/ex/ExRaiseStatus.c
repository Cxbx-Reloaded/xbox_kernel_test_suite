#include <xboxkrnl/xboxkrnl.h>
#include <excpt.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ExRaiseStatus)
{
    TEST_BEGIN();

    // --- STATUS_INTEGER_OVERFLOW ---
    NTSTATUS caught_status = STATUS_SUCCESS;
    BOOLEAN caught = FALSE;

    __try {
        ExRaiseStatus(STATUS_INTEGER_OVERFLOW);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        caught = TRUE;
        caught_status = GetExceptionCode();
    }
    GEN_CHECK(caught, TRUE, "INTEGER_OVERFLOW caught");
    GEN_CHECK(caught_status, STATUS_INTEGER_OVERFLOW, "correct status code");

    // --- STATUS_ACCESS_VIOLATION ---
    caught = FALSE;
    caught_status = 0;
    __try {
        ExRaiseStatus(STATUS_ACCESS_VIOLATION);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        caught = TRUE;
        caught_status = GetExceptionCode();
    }
    GEN_CHECK(caught, TRUE, "ACCESS_VIOLATION caught");
    GEN_CHECK(caught_status, STATUS_ACCESS_VIOLATION, "AV status code");

    // --- STATUS_NO_MEMORY ---
    caught = FALSE;
    caught_status = 0;
    __try {
        ExRaiseStatus(STATUS_NO_MEMORY);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        caught = TRUE;
        caught_status = GetExceptionCode();
    }
    GEN_CHECK(caught, TRUE, "NO_MEMORY caught");
    GEN_CHECK(caught_status, STATUS_NO_MEMORY, "NO_MEMORY code");

    // --- STATUS_UNSUCCESSFUL (generic failure) ---
    caught = FALSE;
    caught_status = 0;
    __try {
        ExRaiseStatus(STATUS_UNSUCCESSFUL);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        caught = TRUE;
        caught_status = GetExceptionCode();
    }
    GEN_CHECK(caught, TRUE, "UNSUCCESSFUL caught");
    GEN_CHECK(caught_status, STATUS_UNSUCCESSFUL, "UNSUCCESSFUL code");

    // --- Custom status code (application-defined) ---
    caught = FALSE;
    caught_status = 0;
    __try {
        ExRaiseStatus((NTSTATUS)0xC0FF0001);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        caught = TRUE;
        caught_status = GetExceptionCode();
    }
    GEN_CHECK(caught, TRUE, "custom status caught");
    GEN_CHECK(caught_status, (NTSTATUS)0xC0FF0001, "custom code preserved");

    // --- Multiple raises in sequence (stack unwind safety) ---
    ULONG count = 0;
    for (ULONG i = 0; i < 10; i++) {
        __try {
            ExRaiseStatus(STATUS_INSUFFICIENT_RESOURCES);
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            count++;
        }
    }
    GEN_CHECK(count, 10, "10 sequential raises all caught");

    // --- Verify exception is non-continuable ---
    // ExRaiseStatus sets EXCEPTION_NONCONTINUABLE. If a filter returns
    // EXCEPTION_CONTINUE_EXECUTION on a non-continuable exception,
    // the system raises STATUS_NONCONTINUABLE_EXCEPTION.
    // SKIP: Testing this would require a nested __except handler that
    // catches the secondary exception, which is fragile and may differ
    // across kernel implementations.

    TEST_END();
}
