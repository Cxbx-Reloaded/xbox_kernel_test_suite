#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/exception.h"
#include "assertions/defines.h"

TEST_FUNC(RtlUnwind)
{
    TEST_BEGIN();

    // RtlUnwind unwinds the stack to a target frame during exception handling.
    // Test it via structured exception handling - the compiler generates
    // RtlUnwind calls when processing __try/__except.
    volatile BOOL reached_handler = FALSE;
    volatile BOOL reached_after_try = FALSE;
    NTSTATUS caught_code = STATUS_SUCCESS;

    __try {
        __try {
            // Raise an exception that will unwind through this frame
            EXCEPTION_RECORD record;
            RtlZeroMemory(&record, sizeof(record));
            record.ExceptionCode = 0xE0000002;
            record.ExceptionFlags = 0;
            record.NumberParameters = 0;
            RtlRaiseException(&record);
            // Should not reach here
            TEST_FAILED();
        }
        __finally {
            // This finally block executes during unwind
            reached_handler = TRUE;
        }
    }
    __except(GetExceptionStatus(&caught_code, EXCEPTION_EXECUTE_HANDLER)) {
        reached_after_try = TRUE;
    }

    GEN_CHECK(reached_handler, TRUE, "finally executed during unwind");
    GEN_CHECK(reached_after_try, TRUE, "except handler reached");
    GEN_CHECK(caught_code, 0xE0000002, "exception code preserved");

    TEST_END();
}
