#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/rtl.h"

TEST_FUNC(RtlInitUnicodeString)
{
    TEST_BEGIN();

    UNICODE_STRING unicode_string;
    WCHAR string[] = L"Xbox";

    unicode_string.Length = 100;
    unicode_string.MaximumLength = 200;
    RtlInitUnicodeString(&unicode_string, NULL);
    assert_unicode_string(
        &unicode_string,
        0,
        0,
        NULL,
        "Src str = NULL, Length = 0"
    );

    RtlInitUnicodeString(&unicode_string, string);
    assert_unicode_string(
        &unicode_string,
        4*2,
        (4+1)*2,
        string,
        "Use char array to create unicode string"
    );

    TEST_END();
}
