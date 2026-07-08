#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(RtlNtStatusToDosError)
{
    TEST_BEGIN();

    typedef struct _status_test {
        NTSTATUS nt_status;
        ULONG expected_dos_error;
        ULONG return_result;
    } status_test;

    status_test tests[] = {
        { .nt_status = STATUS_SUCCESS, .expected_dos_error = ERROR_SUCCESS },
        { .nt_status = STATUS_INVALID_PARAMETER, .expected_dos_error = ERROR_INVALID_PARAMETER },
        { .nt_status = STATUS_NO_MEMORY, .expected_dos_error = ERROR_NOT_ENOUGH_MEMORY },
        { .nt_status = STATUS_BUFFER_OVERFLOW, .expected_dos_error = ERROR_MORE_DATA },
        { .nt_status = STATUS_ACCESS_DENIED, .expected_dos_error = ERROR_ACCESS_DENIED },
        { .nt_status = STATUS_OBJECT_NAME_NOT_FOUND, .expected_dos_error = ERROR_FILE_NOT_FOUND },
        { .nt_status = STATUS_OBJECT_NAME_COLLISION, .expected_dos_error = ERROR_ALREADY_EXISTS },
        { .nt_status = STATUS_INVALID_HANDLE, .expected_dos_error = ERROR_INVALID_HANDLE },
        { .nt_status = STATUS_INSUFFICIENT_RESOURCES, .expected_dos_error = ERROR_NO_SYSTEM_RESOURCES },
        { .nt_status = STATUS_TIMEOUT, .expected_dos_error = WAIT_TIMEOUT },
    };

    for (unsigned i = 0; i < ARRAY_SIZE(tests); i++) {
        tests[i].return_result = RtlNtStatusToDosError(tests[i].nt_status);
    }
    GEN_CHECK_ARRAY_MEMBER(tests, return_result, expected_dos_error, ARRAY_SIZE(tests), "status_tests");

    TEST_END();
}
