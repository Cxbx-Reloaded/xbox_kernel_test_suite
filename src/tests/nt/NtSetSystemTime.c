#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtSetSystemTime)
{
    TEST_BEGIN();

    // Get current time first
    LARGE_INTEGER current_time;
    KeQuerySystemTime(&current_time);
    GEN_CHECK(current_time.QuadPart > 0, TRUE, "current time valid");

    // Set time to current + 1 second, capture previous
    LARGE_INTEGER new_time;
    new_time.QuadPart = current_time.QuadPart + 10000000LL; // +1 second

    LARGE_INTEGER previous_time;
    NTSTATUS status = NtSetSystemTime(&new_time, &previous_time);
    GEN_CHECK(status, STATUS_SUCCESS, "set time +1s");
    GEN_CHECK(previous_time.QuadPart > 0, TRUE, "previous time returned");

    // Verify time moved forward
    LARGE_INTEGER after_set;
    KeQuerySystemTime(&after_set);
    GEN_CHECK(after_set.QuadPart >= new_time.QuadPart, TRUE, "time advanced");

    // Restore original time (approximately)
    status = NtSetSystemTime(&current_time, NULL);
    GEN_CHECK(status, STATUS_SUCCESS, "restore time");

    // NULL PreviousTime is allowed
    new_time.QuadPart = current_time.QuadPart;
    status = NtSetSystemTime(&new_time, NULL);
    GEN_CHECK(status, STATUS_SUCCESS, "NULL prev OK");

    TEST_END();
}
