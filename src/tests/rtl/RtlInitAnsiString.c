#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/rtl.h"

TEST_FUNC(RtlInitAnsiString)
{
    TEST_BEGIN();

    ANSI_STRING ansi_string;
    CHAR string[] = "Xbox";

    ansi_string.Length = 100;
    ansi_string.MaximumLength = 200;
    RtlInitAnsiString(&ansi_string, NULL);
    test_passed &= assert_ansi_string(
        &ansi_string,
        0,
        0,
        NULL,
        "Src str = NULL, Length = 0"
    );

    RtlInitAnsiString(&ansi_string, string);
    test_passed &= assert_ansi_string(
        &ansi_string,
        strlen(string),
        strlen(string) + 1,
        string,
        "Use char array to create ansi string"
    );

    TEST_END();
}
