#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

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

    // Test #1 - time from the epoch
    time.QuadPart = 0;
    RtlTimeToTimeFields(&time, &time_fields);
    GEN_CHECK(time_fields.Year, 1601, "Epoch is January 1st, 1601, 00:00:00:000");
    GEN_CHECK(time_fields.Month, 1, "Epoch is January 1st, 1601, 00:00:00:000");
    GEN_CHECK(time_fields.Day, 1, "Epoch is January 1st, 1601, 00:00:00:000");
    GEN_CHECK(time_fields.Hour, 0, "Epoch is January 1st, 1601, 00:00:00:000");
    GEN_CHECK(time_fields.Minute, 0, "Epoch is January 1st, 1601, 00:00:00:000");
    GEN_CHECK(time_fields.Second, 0, "Epoch is January 1st, 1601, 00:00:00:000");
    GEN_CHECK(time_fields.Millisecond, 0, "Epoch is January 1st, 1601, 00:00:00:000");
    
    // Test #2 - 1 ms from the epoch
    time.QuadPart = 10000;
    RtlTimeToTimeFields(&time, &time_fields);
    GEN_CHECK(time_fields.Year, 1601, "1 ms from the epoch is January 1st, 1601, 00:00:00:001");
    GEN_CHECK(time_fields.Month, 1, "1 ms from the epoch is January 1st, 1601, 00:00:00:001");
    GEN_CHECK(time_fields.Day, 1, "1 ms from the epoch is January 1st, 1601, 00:00:00:001");
    GEN_CHECK(time_fields.Hour, 0, "1 ms from the epoch is January 1st, 1601, 00:00:00:001");
    GEN_CHECK(time_fields.Minute, 0, "1 ms from the epoch is January 1st, 1601, 00:00:00:001");
    GEN_CHECK(time_fields.Second, 0, "1 ms from the epoch is January 1st, 1601, 00:00:00:001");
    GEN_CHECK(time_fields.Millisecond, 1, "1 ms from the epoch is January 1st, 1601, 00:00:00:001");

    // Test #3 - 1 s from the epoch
    time.QuadPart = 10000000;
    RtlTimeToTimeFields(&time, &time_fields);
    GEN_CHECK(time_fields.Year, 1601, "1 s from the epoch is January 1st, 1601, 00:00:01:000");
    GEN_CHECK(time_fields.Month, 1, "1 s from the epoch is January 1st, 1601, 00:00:01:000");
    GEN_CHECK(time_fields.Day, 1, "1 s from the epoch is January 1st, 1601, 00:00:01:000");
    GEN_CHECK(time_fields.Hour, 0, "1 s from the epoch is January 1st, 1601, 00:00:01:000");
    GEN_CHECK(time_fields.Minute, 0, "1 s from the epoch is January 1st, 1601, 00:00:01:000");
    GEN_CHECK(time_fields.Second, 1, "1 s from the epoch is January 1st, 1601, 00:00:01:000");
    GEN_CHECK(time_fields.Millisecond, 0, "1 s from the epoch is January 1st, 1601, 00:00:01:000");

    print_test_footer(func_num, func_name, test_passed);
}

