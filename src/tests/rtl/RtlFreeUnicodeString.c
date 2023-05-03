#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/rtl.h"

void test_RtlFreeUnicodeString()
{
    const char* func_num = "0x011F";
    const char* func_name = "RtlFreeUnicodeString";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const BOOL alloc_buffer = 1;
    ANSI_STRING ansi_string;
    UNICODE_STRING unicode_string;
    char ansi_text[] = "Xbox";

    RtlInitAnsiString(&ansi_string, ansi_text);
    RtlAnsiStringToUnicodeString(&unicode_string, &ansi_string, alloc_buffer);
    RtlFreeUnicodeString(&unicode_string);
    tests_passed &= assert_unicode_string(
        &unicode_string,
        0,
        0,
        NULL,
        "Free unicode string"
    );

    print_test_footer(func_num, func_name, tests_passed);
}
