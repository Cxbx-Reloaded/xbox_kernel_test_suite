#include <xboxkrnl/xboxkrnl.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>

#include "util/output.h"
#include "assertions/common.h"
#include "assertions/rtl.h"

// Odd behavior of this function: A terminating NULL character '\0' is not added to the end of the appended string.
TEST_FUNC(RtlAppendStringToString)
{
    TEST_BEGIN();

    // Append string to empty dest once
    ANSI_STRING src_str, dest_str;
    RtlInitAnsiString(&src_str, "XBOX");
    CHAR dest_buffer[src_str.Length * 2];
    CHAR expected_result[30];

    dest_str.Length = 0;
    dest_str.MaximumLength = sizeof(dest_buffer);
    dest_str.Buffer = dest_buffer;
    strncpy(expected_result, src_str.Buffer, src_str.MaximumLength);

    NTSTATUS ret = RtlAppendStringToString(&dest_str, &src_str);
    assert_NTSTATUS(ret, STATUS_SUCCESS, api_name);
    dest_buffer[dest_str.Length] = '\0'; // RtlAppendStringToString does not add a terminating NULL character which breaks strncmp
    assert_ansi_string(
        &dest_str,
        strlen(expected_result),
        sizeof(dest_buffer),
        expected_result,
        "Append src_str to dest_str"
    );

    // if (src_str.Length + dest_str.Length) <= dest_str.MaximumLength, then return STATUS_SUCCESS
    // and append the strings. This will test the equal to case.
    strncat(expected_result, src_str.Buffer, src_str.MaximumLength);

    ret = RtlAppendStringToString(&dest_str, &src_str);
    assert_NTSTATUS(ret, STATUS_SUCCESS, api_name);
    assert_ansi_string(
        &dest_str,
        strlen(expected_result),
        sizeof(dest_buffer),
        expected_result,
        "Append src_str to dest_str a second time"
    );

    // Should now return STATUS_BUFFER_TOO_SMALL
    RtlInitAnsiString(&src_str, "A");
    ret = RtlAppendStringToString(&dest_str, &src_str);
    assert_NTSTATUS(ret, STATUS_BUFFER_TOO_SMALL, api_name);

    // Appending empty str is okay though
    RtlInitAnsiString(&src_str, "");
    ret = RtlAppendStringToString(&dest_str, &src_str);
    assert_NTSTATUS(ret, STATUS_SUCCESS, api_name);
    assert_ansi_string(
        &dest_str,
        strlen(expected_result),
        sizeof(dest_buffer),
        expected_result,
        "Appending empty_str does not change dest_str"
    );

    TEST_END();
}

TEST_FUNC(RtlAppendUnicodeStringToString)
{
    TEST_BEGIN();

    UNICODE_STRING src_str, dest_str;
    RtlInitUnicodeString(&src_str, L"Xbox");
    const size_t num_chars_in_src = wcslen(src_str.Buffer);
    WCHAR buffer[(num_chars_in_src + 1) * 2]; // Add 1 to hold the \0 end character
    WCHAR expected_result[(num_chars_in_src + 1) * 2];
    const size_t num_buf_bytes = sizeof(buffer);

    dest_str.Length = 0;
    dest_str.MaximumLength = num_buf_bytes;
    dest_str.Buffer = buffer;
    wcsncpy(expected_result, src_str.Buffer, num_chars_in_src + 1);

    NTSTATUS ret = RtlAppendUnicodeStringToString(&dest_str, &src_str);
    assert_NTSTATUS(ret, STATUS_SUCCESS, api_name);
    assert_unicode_string(
        &dest_str,
        wcslen(expected_result) * sizeof(WCHAR),
        num_buf_bytes,
        expected_result,
        "Append src str to empty dest str"
    );

    wcscat(expected_result, src_str.Buffer);
    ret = RtlAppendUnicodeStringToString(&dest_str, &src_str);
    assert_NTSTATUS(ret, STATUS_SUCCESS, api_name);
    assert_unicode_string(
        &dest_str,
        wcslen(expected_result) * sizeof(WCHAR),
        num_buf_bytes,
        expected_result,
        "Append src str to dest str again"
    );

    ret = RtlAppendUnicodeStringToString(&dest_str, &src_str);
    assert_NTSTATUS(ret, STATUS_BUFFER_TOO_SMALL, api_name);

    TEST_END();
}

TEST_FUNC(RtlAppendUnicodeToString)
{
    TEST_BEGIN();

    const WCHAR* src_text = L"Xbox";
    const uint8_t num_chars_in_src = wcslen(src_text) + 1;
    WCHAR buffer[num_chars_in_src * 2];
    WCHAR expected_result[num_chars_in_src * 2];
    const uint8_t num_buf_bytes = sizeof(buffer);
    UNICODE_STRING dest_str;

    dest_str.Length = 0;
    dest_str.MaximumLength = num_buf_bytes;
    dest_str.Buffer = buffer;
    wcsncpy(expected_result, src_text, num_chars_in_src);

    NTSTATUS ret = RtlAppendUnicodeToString(&dest_str, src_text);
    assert_NTSTATUS(ret, STATUS_SUCCESS, api_name);
    assert_unicode_string(
        &dest_str,
        wcslen(expected_result) * sizeof(WCHAR),
        num_buf_bytes,
        expected_result,
        "Append src str to empty dest str"
    );

    wcsncat(expected_result, src_text, num_chars_in_src);
    ret = RtlAppendUnicodeToString(&dest_str, src_text);
    assert_NTSTATUS(ret, STATUS_SUCCESS, api_name);
    assert_unicode_string(
        &dest_str,
        wcslen(expected_result) * sizeof(WCHAR),
        num_buf_bytes,
        expected_result,
        "Append src str to dest str again"
    );

    ret = RtlAppendUnicodeToString(&dest_str, src_text);
    assert_NTSTATUS(ret, STATUS_BUFFER_TOO_SMALL, api_name);

    TEST_END();
}
