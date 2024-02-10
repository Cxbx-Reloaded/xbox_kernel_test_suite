#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/rtl.h"

void test_RtlInitUnicodeString(int func_num, const char* func_name)
{
    BOOL tests_passed = 1;
    UNICODE_STRING unicode_string;
    WCHAR string[] = L"Xbox";
    print_test_header(func_num, func_name);

    unicode_string.Length = 100;
    unicode_string.MaximumLength = 200;
    RtlInitUnicodeString(&unicode_string, NULL);
    tests_passed &= assert_unicode_string(
        &unicode_string,
        0,
        0,
        NULL,
        "Src str = NULL, Length = 0"
    );

    RtlInitUnicodeString(&unicode_string, string);
    tests_passed &= assert_unicode_string(
        &unicode_string,
        4*2,
        (4+1)*2,
        string,
        "Use char array to create unicode string"
    );

    print_test_footer(func_num, func_name, tests_passed);
}
