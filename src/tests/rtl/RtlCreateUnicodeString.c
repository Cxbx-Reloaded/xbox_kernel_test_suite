#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"
#include "assertions/rtl.h"

TEST_FUNC(RtlCreateUnicodeString)
{
    TEST_BEGIN();

    // RtlCreateUnicodeString allocates a buffer and copies the source string
    UNICODE_STRING str = { 0 };
    WCHAR source[] = L"Xbox";
    BOOLEAN result;

    // Create from a normal string
    result = RtlCreateUnicodeString(&str, source);
    GEN_CHECK(result, TRUE, "create returned TRUE");
    if (result) {
        // Length should be 4 chars * 2 bytes = 8
        GEN_CHECK(str.Length, 8, "Length");
        // MaximumLength includes null terminator = (4+1)*2 = 10
        GEN_CHECK(str.MaximumLength, 10, "MaximumLength");
        // Buffer should be newly allocated (not same as source)
        GEN_CHECK(str.Buffer != source, TRUE, "Buffer is new allocation");
        GEN_CHECK(str.Buffer != NULL, TRUE, "Buffer not NULL");
        // Content should match
        if (str.Buffer) {
            GEN_CHECK(str.Buffer[0], L'X', "Buffer[0]");
            GEN_CHECK(str.Buffer[3], L'x', "Buffer[3]");
            GEN_CHECK(str.Buffer[4], L'\0', "null terminated");
        }
        RtlFreeUnicodeString(&str);
    }

    // Create from empty string
    str.Length = 0;
    str.MaximumLength = 0;
    str.Buffer = NULL;
    result = RtlCreateUnicodeString(&str, L"");
    GEN_CHECK(result, TRUE, "create empty returned TRUE");
    if (result) {
        GEN_CHECK(str.Length, 0, "empty Length");
        GEN_CHECK(str.MaximumLength, 2, "empty MaximumLength");
        GEN_CHECK(str.Buffer != NULL, TRUE, "empty Buffer not NULL");
        if (str.Buffer) {
            GEN_CHECK(str.Buffer[0], L'\0', "empty null terminated");
        }
        RtlFreeUnicodeString(&str);
    }

    TEST_END();
}
