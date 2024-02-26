#include <xboxkrnl/xboxkrnl.h>
#include <stdint.h>

#include "util/output.h"
#include "assertions/rtl.h"

TEST_FUNC(RtlCopyUnicodeString)
{
    TEST_BEGIN();

    UNICODE_STRING dest_str, shorter_str, longer_str;
    WCHAR buffer[8];
    const uint8_t num_buf_bytes = sizeof(buffer);

    RtlInitUnicodeString(&shorter_str, L"Xbox");
    RtlInitUnicodeString(&longer_str, L"Xbox Original");
    dest_str.Length = 8;
    dest_str.MaximumLength = num_buf_bytes;
    dest_str.Buffer = buffer;

    RtlCopyUnicodeString(&dest_str, NULL);
    assert_unicode_string(&dest_str, 0, num_buf_bytes, buffer, "NULL src_str");

    dest_str.Length = 0;
    RtlCopyUnicodeString(&dest_str, &shorter_str);
    assert_unicode_string(
        &dest_str,
        shorter_str.Length,
        num_buf_bytes,
        buffer,
        "Copy shorter str into str"
    );

    dest_str.Length = 0;
    RtlCopyUnicodeString(&dest_str, &longer_str);
    assert_unicode_string(
        &dest_str,
        num_buf_bytes,
        num_buf_bytes,
        buffer,
        "Copy longer str into str"
    );

    TEST_END();
}
