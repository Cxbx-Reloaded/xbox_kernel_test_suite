#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"
#include "util/misc.h"

void test_RtlTimeFieldsToTime()
{
    const char* func_num = "0x0130";
    const char* func_name = "RtlTimeFieldsToTime";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    TIME_FIELDS time_fields;
    LARGE_INTEGER time;

    // Test #1 - time from the epoch
    time_fields.Year = 1601;
    time_fields.Month = 1;
    time_fields.Day = 1;
    time_fields.Hour = 0;
    time_fields.Minute = 0;
    time_fields.Second = 0;
    time_fields.Millisecond = 0;
    time_fields.Weekday = 0; // value ignored by the function
    BOOLEAN ok = RtlTimeFieldsToTime(&time_fields, &time);
    GEN_CHECK(ok, TRUE, "Time converted correctly");
    if (ok) {
        GEN_CHECK(time.QuadPart, 0, "Epoch time is zero");
    }
    
    // Test #2 - 1 ms from the epoch
    time_fields.Year = 1601;
    time_fields.Month = 1;
    time_fields.Day = 1;
    time_fields.Hour = 0;
    time_fields.Minute = 0;
    time_fields.Second = 0;
    time_fields.Millisecond = 1;
    time_fields.Weekday = 0;
    ok = RtlTimeFieldsToTime(&time_fields, &time);
    GEN_CHECK(ok, TRUE, "Time converted correctly");
    if (ok) {
        GEN_CHECK(time.QuadPart, 10000, "1 ms from the epoch time is 10000");
    }

    // Test #3 - 1 s from the epoch
    time_fields.Year = 1601;
    time_fields.Month = 1;
    time_fields.Day = 1;
    time_fields.Hour = 0;
    time_fields.Minute = 0;
    time_fields.Second = 1;
    time_fields.Millisecond = 0;
    time_fields.Weekday = 0;
    ok = RtlTimeFieldsToTime(&time_fields, &time);
    GEN_CHECK(ok, TRUE, "Time converted correctly");
    if (ok) {
        GEN_CHECK(time.QuadPart, 10000000, "1 s from the epoch time is 10000000");
    }

    // NOTE: Keep above tests (1-3) separated to confirm that the function calculates the time in 100 ns time intervals

    // Test #4 - expected range of acceptable and unacceptable values
    typedef struct _time_test {
        const TIME_FIELDS time_fields;
        const BOOLEAN result_expected;
        const LARGE_INTEGER time_expected;
        LARGE_INTEGER time_actual;
        BOOLEAN result_actual;
    } time_test;

    time_test time_tests[] = {
        { .time_fields = { 0 }, .result_expected = FALSE },
        // Verify the year do fail by go one year earlier
        { .time_fields = { .Year = 1600, .Month = 1, .Day = 1 }, .result_expected = FALSE },
        { .time_fields = { .Year = 1601, .Month = 1, .Day = 1 }, .result_expected = TRUE, .time_expected.QuadPart = 0 },
        // Then check Month/Day fields cannot be zero
        { .time_fields = { .Year = 1601, .Month = 0, .Day = 1 }, .result_expected = FALSE },
        { .time_fields = { .Year = 1601, .Month = 1, .Day = 0 }, .result_expected = FALSE },
        // Weekday variable is ignored by the function so it should still succeed
        { .time_fields = { .Year = 1601, .Month = 1, .Day = 1, .Weekday = 20 }, .result_expected = TRUE },
        { .time_fields = { .Year = 1601, .Month = 1, .Day = 1, .Weekday = 7 }, .result_expected = TRUE },
        { .time_fields = { .Year = 1601, .Month = 1, .Day = 1, .Weekday = 6 }, .result_expected = TRUE },
        // Make each field, Month/Day/Hour/etc, fail by go over one then pull back by one to confirm passing
        { .time_fields = { .Year = 1601, .Month = 1, .Day = 32 }, .result_expected = FALSE },
        { .time_fields = { .Year = 1601, .Month = 1, .Day = 31 }, .result_expected = TRUE, .time_expected.QuadPart = 25920000000000 },
        { .time_fields = { .Year = 1601, .Month = 1, .Day = 1, .Hour = 24 }, .result_expected = FALSE },
        { .time_fields = { .Year = 1601, .Month = 1, .Day = 1, .Hour = 23 }, .result_expected = TRUE, .time_expected.QuadPart = 828000000000 },
        { .time_fields = { .Year = 1601, .Month = 1, .Day = 1, .Minute = 60 }, .result_expected = FALSE },
        { .time_fields = { .Year = 1601, .Month = 1, .Day = 1, .Minute = 59 }, .result_expected = TRUE, .time_expected.QuadPart = 35400000000 },
        { .time_fields = { .Year = 1601, .Month = 1, .Day = 1, .Second = 60 }, .result_expected = FALSE },
        { .time_fields = { .Year = 1601, .Month = 1, .Day = 1, .Second = 59 }, .result_expected = TRUE, .time_expected.QuadPart = 590000000 },
        { .time_fields = { .Year = 1601, .Month = 1, .Day = 1, .Millisecond = 1000 }, .result_expected = FALSE },
        { .time_fields = { .Year = 1601, .Month = 1, .Day = 1, .Millisecond = 999 }, .result_expected = TRUE, .time_expected.QuadPart = 9990000 },
        // Check that leap year is working as intended for both fail and success
        { .time_fields = { .Year = 1603, .Month = 2, .Day = 29 }, .result_expected = FALSE },
        { .time_fields = { .Year = 1603, .Month = 2, .Day = 28 }, .result_expected = TRUE, .time_expected.QuadPart = 680832000000000 },
        { .time_fields = { .Year = 1604, .Month = 2, .Day = 30 }, .result_expected = FALSE },
        { .time_fields = { .Year = 1604, .Month = 2, .Day = 29 }, .result_expected = TRUE, .time_expected.QuadPart = 997056000000000 },
        // Check large integer max (max is actually 9223372036854770000 since last 4 digits are always 0.)
        { .time_fields = { .Year = 30828, .Month = 9, .Day = 14, .Hour = 2, .Minute = 48, .Second = 5, .Millisecond = 477 }, .result_expected = TRUE, .time_expected.QuadPart = 9223372036854770000 },
        // Basically overflow max test
        { .time_fields = { .Year = 30828, .Month = 9, .Day = 14, .Hour = 2, .Minute = 48, .Second = 5, .Millisecond = 478 }, .result_expected = TRUE, .time_expected.QuadPart = -9223372036854771616 },
    };
    enum { time_tests_size = ARRAY_SIZE(time_tests) };

    for (unsigned i = 0; i < time_tests_size; i++) {
        // NOTE: PTIME_FIELDS cast is used to bypass warning from compiler
        time_tests[i].result_actual = RtlTimeFieldsToTime((PTIME_FIELDS)&time_tests[i].time_fields, &time_tests[i].time_actual);
    }
    GEN_CHECK_ARRAY_MEMBER(time_tests, time_actual.QuadPart, time_expected.QuadPart, time_tests_size, "time_tests");
    GEN_CHECK_ARRAY_MEMBER(time_tests, result_actual, result_expected, time_tests_size, "time_tests");

    print_test_footer(func_num, func_name, test_passed);
}

void test_RtlTimeToTimeFields()
{
    const char* func_num = "0x0131";
    const char* func_name = "RtlTimeToTimeFields";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    TIME_FIELDS time_fields;
    LARGE_INTEGER time;

    // Test #1 - expected range of acceptable and unacceptable values
    typedef struct _time_fields_test {
        const TIME_FIELDS expected;
        const LARGE_INTEGER time;
        TIME_FIELDS actual;
    } time_fields_test;

    time_fields_test time_fields_tests[] = {
        { .expected = { .Year = 1601, .Month = 1, .Day = 1, .Weekday = 1 }, .time.QuadPart = 0 },
        { .expected = { .Year = 1601, .Month = 1, .Day = 1, .Millisecond = 1, .Weekday = 1 }, .time.QuadPart = 10000 },
        { .expected = { .Year = 1601, .Month = 1, .Day = 1, .Second = 1, .Weekday = 1 }, .time.QuadPart = 10000000 },
        { .expected = { .Year = 1601, .Month = 1, .Day = 31, .Weekday = 3 }, .time.QuadPart = 25920000000000 },
        { .expected = { .Year = 1601, .Month = 1, .Day = 1, .Hour = 23, .Weekday = 1 }, .time.QuadPart = 828000000000 },
        { .expected = { .Year = 1601, .Month = 1, .Day = 1, .Minute = 59, .Weekday = 1 }, .time.QuadPart = 35400000000 },
        { .expected = { .Year = 1601, .Month = 1, .Day = 1, .Second = 59, .Weekday = 1 }, .time.QuadPart = 590000000 },
        { .expected = { .Year = 1601, .Month = 1, .Day = 1, .Millisecond = 999, .Weekday = 1 }, .time.QuadPart = 9990000 },
        // Check leap year difference
        { .expected = { .Year = 1603, .Month = 2, .Day = 28 }, .time.QuadPart = 680832000000000 },
        { .expected = { .Year = 1604, .Month = 2, .Day = 29, .Weekday = 5 }, .time.QuadPart = 997056000000000 },
        // Check large integer max (max is actually 9223372036854770000 since last 4 digits are always 0.)
        { .expected = { .Year = 30828, .Month = 9, .Day = 14, .Hour = 2, .Minute = 48, .Second = 5, .Millisecond = 477, .Weekday = 4 }, .time.QuadPart = 9223372036854770000 },
        { .expected = { .Year = 30828, .Month = 9, .Day = 14, .Hour = 2, .Minute = 48, .Second = 5, .Millisecond = 477, .Weekday = 4 }, .time.QuadPart = 9223372036854775807 },
        // Basically overflow max test
        { .expected = { .Year = 650, .Month = 5, .Day = 10, .Hour = 1190, .Minute = 14, .Second = 41, .Millisecond = 819, .Weekday = 2 }, .time.QuadPart = -9223372036854771616 },
    };
    enum { time_fields_tests_size = ARRAY_SIZE(time_fields_tests) };

    for (unsigned i = 0; i < time_fields_tests_size; i++) {
        // NOTE: PLARGE_INTEGER cast is used to bypass warning from compiler
        RtlTimeToTimeFields((PLARGE_INTEGER)&time_fields_tests[i].time, &time_fields_tests[i].actual);
    }
    GEN_CHECK_ARRAY_MEMBER(time_fields_tests, actual.Year, expected.Year, time_fields_tests_size, "time_fields_tests");
    GEN_CHECK_ARRAY_MEMBER(time_fields_tests, actual.Month, expected.Month, time_fields_tests_size, "time_fields_tests");
    GEN_CHECK_ARRAY_MEMBER(time_fields_tests, actual.Day, expected.Day, time_fields_tests_size, "time_fields_tests");
    GEN_CHECK_ARRAY_MEMBER(time_fields_tests, actual.Hour, expected.Hour, time_fields_tests_size, "time_fields_tests");
    GEN_CHECK_ARRAY_MEMBER(time_fields_tests, actual.Minute, expected.Minute, time_fields_tests_size, "time_fields_tests");
    GEN_CHECK_ARRAY_MEMBER(time_fields_tests, actual.Second, expected.Second, time_fields_tests_size, "time_fields_tests");
    GEN_CHECK_ARRAY_MEMBER(time_fields_tests, actual.Millisecond, expected.Millisecond, time_fields_tests_size, "time_fields_tests");

    print_test_footer(func_num, func_name, test_passed);
}

