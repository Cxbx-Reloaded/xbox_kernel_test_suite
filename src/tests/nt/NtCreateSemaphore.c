#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(NtCreateSemaphore)
{
    TEST_BEGIN();

    typedef struct {
        const char* description;
        LONG initial_count;
        LONG maximum_count;
        NTSTATUS expected_status;
    } sem_test_entry;

    sem_test_entry success_tests[] = {
        { "count 0, max 1",   0, 1,        STATUS_SUCCESS },
        { "count 1, max 1",   1, 1,        STATUS_SUCCESS },
        { "count 0, max 100", 0, 100,      STATUS_SUCCESS },
        { "count 50, max 100",50, 100,     STATUS_SUCCESS },
        { "count=max large",  0x7FFFFFFF, 0x7FFFFFFF, STATUS_SUCCESS },
    };

    sem_test_entry fail_tests[] = {
        { "initial > max",       5,  2,  STATUS_INVALID_PARAMETER },
        { "max 0",              0,  0,  STATUS_INVALID_PARAMETER },
        { "negative initial",  -1,  5,  STATUS_INVALID_PARAMETER },
        { "negative max",       0, -1,  STATUS_INVALID_PARAMETER },
    };

    // --- Success cases ---
    for (unsigned i = 0; i < ARRAY_SIZE(success_tests); i++) {
        HANDLE handle = NULL;
        NTSTATUS status = NtCreateSemaphore(&handle, NULL,
            success_tests[i].initial_count, success_tests[i].maximum_count);
        GEN_CHECK(status, success_tests[i].expected_status, success_tests[i].description);

        if (NT_SUCCESS(status)) {
            GEN_CHECK(handle != NULL, TRUE, success_tests[i].description);

            SEMAPHORE_BASIC_INFORMATION info;
            NtQuerySemaphore(handle, &info);
            GEN_CHECK(info.CurrentCount, success_tests[i].initial_count, success_tests[i].description);
            GEN_CHECK(info.MaximumCount, success_tests[i].maximum_count, success_tests[i].description);

            NtClose(handle);
        }
    }

    // --- Failure cases ---
    for (unsigned i = 0; i < ARRAY_SIZE(fail_tests); i++) {
        HANDLE handle = NULL;
        NTSTATUS status = NtCreateSemaphore(&handle, NULL,
            fail_tests[i].initial_count, fail_tests[i].maximum_count);
        GEN_CHECK(status, fail_tests[i].expected_status, fail_tests[i].description);
    }

    TEST_END();
}
