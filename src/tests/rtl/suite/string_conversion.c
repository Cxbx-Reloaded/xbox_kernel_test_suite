#include <xboxkrnl/xboxkrnl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"
#include "assertions/common.h"
#include "assertions/rtl.h"

TEST_FUNC(RtlAnsiStringToUnicodeString)
{
    TEST_BEGIN();

    const uint32_t long_str_size = 0x10000;
    UNICODE_STRING dest_str = { 0 };
    ANSI_STRING src_str = { 0 };
    CHAR* long_str = malloc(sizeof(CHAR) * long_str_size);
    if(long_str == NULL) {
        print("ERROR: Could not malloc long_str");
    }

    NTSTATUS ret = RtlAnsiStringToUnicodeString(&dest_str, &src_str, 0);
    assert_NTSTATUS(ret, STATUS_BUFFER_OVERFLOW, api_name);

    memset(long_str, 'a', long_str_size);
    long_str[long_str_size - 1] = '\0';
    RtlInitAnsiString(&src_str, long_str);
    ret = RtlAnsiStringToUnicodeString(&dest_str, &src_str, 0);
    assert_NTSTATUS(ret, STATUS_INVALID_PARAMETER_2, api_name);
    free(long_str);

    TEST_END();
}

TEST_FUNC(RtlCharToInteger)
{
    TEST_BEGIN();

    // if base != (2 | 8 | 10 | 16) return STATUS_INVALID_PARAMETER
    // SPECIAL CASE: Base = 0 will not throw this error as base = 0 signifies that the base will be included
    // in the input string: '0b' = binary, '0o' = octal, '0x' = hex
    NTSTATUS ret = RtlCharToInteger("1", 1, NULL);
    assert_NTSTATUS(ret, STATUS_INVALID_PARAMETER, api_name);

    typedef struct _char_to_int_test {
        CHAR* const input;
        CHAR* const base_format;
        const ULONG base;
        const ULONG expected_value;
        const NTSTATUS expected_return;
        ULONG neg_expected_value;
        // call's results
        ULONG base_value;
        ULONG neg_base_value;
        ULONG format_value;
        ULONG neg_format_value;
        NTSTATUS base_ret;
        NTSTATUS neg_base_ret;
        NTSTATUS format_ret;
        NTSTATUS neg_format_ret;
    } char_to_int_test;

    // Test all of the different bases with positive and negative results, including base = 0.
    // In test cases where there are invalid numbers for the specified base, the RtlCharToInteger will
    // convert the valid numbers up to invalid number.
    // For example, base = 2, input = '1015C' will return 0x5
    char_to_int_test char_to_int_tests[] = {
        { .input = "11001010", .base_format = "0b", .base = 2, .expected_value = 0xCA, .expected_return = STATUS_SUCCESS },
        { .input = "7631", .base_format = "0o", .base = 8, .expected_value = 0xF99, .expected_return = STATUS_SUCCESS },
        { .input = "1100", .base_format = "", .base = 10, .expected_value = 1100, .expected_return = STATUS_SUCCESS },
        { .input = "5FAC2", .base_format = "0x", .base = 16, .expected_value = 0x5FAC2, .expected_return = STATUS_SUCCESS },
        { .input = "101C813", .base_format = "0b", .base = 2, .expected_value = 0x5, .expected_return = STATUS_SUCCESS },
        { .input = "76BA787", .base_format = "0o", .base = 8, .expected_value = 0x3E, .expected_return = STATUS_SUCCESS },
        { .input = "1000B1", .base_format = "", .base = 10, .expected_value = 1000, .expected_return = STATUS_SUCCESS },
    };
    enum { char_to_int_tests_size = ARRAY_SIZE(char_to_int_tests) };

    CHAR buffer[50];
    for (unsigned i = 0; i < char_to_int_tests_size; i++) {
        // Easier way to convert provided expected_value to negative without need to manually input for each one.
        char_to_int_tests[i].neg_expected_value = -1 * char_to_int_tests[i].expected_value;

        // Use base input directly test
        char_to_int_tests[i].base_ret = RtlCharToInteger(char_to_int_tests[i].input, char_to_int_tests[i].base, &char_to_int_tests[i].base_value);

        // Convert base input to negative input test
        strcpy(buffer, "-");
        strcat(buffer, char_to_int_tests[i].input);
        char_to_int_tests[i].neg_base_ret = RtlCharToInteger(buffer, char_to_int_tests[i].base, &char_to_int_tests[i].neg_base_value);

        // Use format input directly test
        strcpy(buffer, char_to_int_tests[i].base_format);
        strcat(buffer, char_to_int_tests[i].input);
        char_to_int_tests[i].format_ret = RtlCharToInteger(buffer, 0, &char_to_int_tests[i].format_value);

        // Convert format input to negative format input test
        strcpy(buffer, "-");
        strcat(buffer, char_to_int_tests[i].base_format);
        strcat(buffer, char_to_int_tests[i].input);
        char_to_int_tests[i].neg_format_ret = RtlCharToInteger(buffer, 0, &char_to_int_tests[i].neg_format_value);
    }
    // Verify base input's output
    GEN_CHECK_ARRAY_MEMBER(char_to_int_tests, base_value, expected_value, char_to_int_tests_size, "char_to_int_tests");
    GEN_CHECK_ARRAY_MEMBER(char_to_int_tests, base_ret, expected_return, char_to_int_tests_size, "char_to_int_tests");
    GEN_CHECK_ARRAY_MEMBER(char_to_int_tests, neg_base_value, neg_expected_value, char_to_int_tests_size, "char_to_int_tests");
    GEN_CHECK_ARRAY_MEMBER(char_to_int_tests, neg_base_ret, expected_return, char_to_int_tests_size, "char_to_int_tests");
    // Verify format input's output
    GEN_CHECK_ARRAY_MEMBER(char_to_int_tests, format_value, expected_value, char_to_int_tests_size, "char_to_int_tests");
    GEN_CHECK_ARRAY_MEMBER(char_to_int_tests, format_ret, expected_return, char_to_int_tests_size, "char_to_int_tests");
    GEN_CHECK_ARRAY_MEMBER(char_to_int_tests, neg_format_value, neg_expected_value, char_to_int_tests_size, "char_to_int_tests");
    GEN_CHECK_ARRAY_MEMBER(char_to_int_tests, neg_format_ret, expected_return, char_to_int_tests_size, "char_to_int_tests");

    TEST_END();
}

TEST_FUNC(RtlIntegerToChar)
{
    TEST_BEGIN();

    typedef struct _int_to_char_test {
        ULONG value;
        ULONG base;
        ULONG buf_len;
        const char* expected;
        NTSTATUS expected_status;
    } int_to_char_test;

    int_to_char_test tests[] = {
        { .value = 0, .base = 10, .buf_len = 16, .expected = "0", .expected_status = STATUS_SUCCESS },
        { .value = 255, .base = 16, .buf_len = 16, .expected = "FF", .expected_status = STATUS_SUCCESS },
        { .value = 255, .base = 10, .buf_len = 16, .expected = "255", .expected_status = STATUS_SUCCESS },
        { .value = 10, .base = 2, .buf_len = 16, .expected = "1010", .expected_status = STATUS_SUCCESS },
        { .value = 8, .base = 8, .buf_len = 16, .expected = "10", .expected_status = STATUS_SUCCESS },
        { .value = 12345, .base = 10, .buf_len = 16, .expected = "12345", .expected_status = STATUS_SUCCESS },
        // Buffer too small
        { .value = 12345, .base = 10, .buf_len = 2, .expected = "", .expected_status = STATUS_BUFFER_OVERFLOW },
    };

    for (unsigned i = 0; i < ARRAY_SIZE(tests); i++) {
        CHAR buf[16] = { 0 };
        NTSTATUS status = RtlIntegerToChar(tests[i].value, tests[i].base, tests[i].buf_len, buf);
        assert_NTSTATUS(status, tests[i].expected_status, "RtlIntegerToChar");
        if (NT_SUCCESS(status)) {
            BOOL match = (strcmp(buf, tests[i].expected) == 0);
            if (!match) {
                print("  ERROR(line %d): Expected '%s', Got '%s'", __LINE__, tests[i].expected, buf);
                TEST_FAILED();
            }
        }
    }

    TEST_END();
}

TEST_FUNC(RtlIntegerToUnicodeString)
{
    TEST_BEGIN();

    UNICODE_STRING str;
    WCHAR buf[32];
    str.Buffer = buf;
    str.MaximumLength = sizeof(buf);
    str.Length = 0;
    NTSTATUS status;

    // Base 10
    status = RtlIntegerToUnicodeString(12345, 10, &str);
    GEN_CHECK(status, STATUS_SUCCESS, "base10 status");
    GEN_CHECK(str.Length, 10, "base10 Length"); // 5 chars * 2 bytes

    // Base 16
    str.Length = 0;
    status = RtlIntegerToUnicodeString(0xFF, 16, &str);
    GEN_CHECK(status, STATUS_SUCCESS, "base16 status");
    GEN_CHECK(str.Length, 4, "base16 Length"); // "FF" = 2 chars * 2 bytes

    // Base 0 defaults to base 10
    str.Length = 0;
    status = RtlIntegerToUnicodeString(100, 0, &str);
    GEN_CHECK(status, STATUS_SUCCESS, "base0 status");
    GEN_CHECK(str.Length, 6, "base0 Length"); // "100" = 3 chars * 2 bytes

    // Zero value
    str.Length = 0;
    status = RtlIntegerToUnicodeString(0, 10, &str);
    GEN_CHECK(status, STATUS_SUCCESS, "zero status");
    GEN_CHECK(str.Length, 2, "zero Length"); // "0" = 1 char * 2 bytes

    // Buffer too small
    WCHAR tiny_buf[2];
    str.Buffer = tiny_buf;
    str.MaximumLength = sizeof(tiny_buf);
    str.Length = 0;
    status = RtlIntegerToUnicodeString(12345, 10, &str);
    GEN_CHECK(status, STATUS_BUFFER_OVERFLOW, "overflow status");

    TEST_END();
}

TEST_FUNC(RtlMultiByteToUnicodeN)
{
    TEST_BEGIN();

    CHAR src[] = "Xbox";
    WCHAR dest[16] = { 0 };
    ULONG bytes_written = 0;
    NTSTATUS status;

    // Normal conversion
    status = RtlMultiByteToUnicodeN(dest, sizeof(dest), &bytes_written, src, 4);
    GEN_CHECK(status, STATUS_SUCCESS, "normal status");
    GEN_CHECK(bytes_written, 8, "bytes_written"); // 4 chars * 2 bytes
    GEN_CHECK(dest[0], L'X', "dest[0]");
    GEN_CHECK(dest[1], L'b', "dest[1]");
    GEN_CHECK(dest[2], L'o', "dest[2]");
    GEN_CHECK(dest[3], L'x', "dest[3]");

    // Limited output buffer
    RtlZeroMemory(dest, sizeof(dest));
    bytes_written = 0;
    status = RtlMultiByteToUnicodeN(dest, 4, &bytes_written, src, 4);
    GEN_CHECK(status, STATUS_BUFFER_OVERFLOW, "limited status");
    GEN_CHECK(bytes_written, 4, "limited bytes_written");
    GEN_CHECK(dest[0], L'X', "limited dest[0]");
    GEN_CHECK(dest[1], L'b', "limited dest[1]");

    // Empty source
    RtlZeroMemory(dest, sizeof(dest));
    bytes_written = 0;
    status = RtlMultiByteToUnicodeN(dest, sizeof(dest), &bytes_written, src, 0);
    GEN_CHECK(status, STATUS_SUCCESS, "empty status");
    GEN_CHECK(bytes_written, 0, "empty bytes_written");

    TEST_END();
}

TEST_FUNC(RtlMultiByteToUnicodeSize)
{
    TEST_BEGIN();

    ULONG unicode_size = 0;
    NTSTATUS status;

    // Each multibyte char maps to 2 bytes of unicode
    status = RtlMultiByteToUnicodeSize(&unicode_size, "Xbox", 4);
    GEN_CHECK(status, STATUS_SUCCESS, "status");
    GEN_CHECK(unicode_size, 8, "size for 4 chars");

    status = RtlMultiByteToUnicodeSize(&unicode_size, "", 0);
    GEN_CHECK(status, STATUS_SUCCESS, "empty status");
    GEN_CHECK(unicode_size, 0, "size for 0 chars");

    status = RtlMultiByteToUnicodeSize(&unicode_size, "A", 1);
    GEN_CHECK(status, STATUS_SUCCESS, "single status");
    GEN_CHECK(unicode_size, 2, "size for 1 char");

    TEST_END();
}

TEST_FUNC(RtlUnicodeStringToAnsiString)
{
    TEST_BEGIN();

    UNICODE_STRING unicode_string;
    WCHAR unicode_text[] = L"Xbox\x0100\xFFFF\0Xbox\x0255";
    char ansi_text[] = "Xbox??\0Xbox?";
    char ansi_buffer[sizeof(ansi_text)];
    ANSI_STRING ansi_string;
    const BOOL alloc_buffer = 1;

    // Test if default behavior is working as intended.
    RtlInitUnicodeString(&unicode_string, unicode_text);
    assert_unicode_string(
        &unicode_string,
        wcslen(unicode_text) * sizeof(WCHAR),
        (wcslen(unicode_text) + 1) * sizeof(WCHAR),
        unicode_text,
        "Initialize unicode string."
    );

    // Test default behavior for allocated buffer.
    NTSTATUS result = RtlUnicodeStringToAnsiString(&ansi_string, &unicode_string, alloc_buffer);
    assert_NTSTATUS(result, STATUS_SUCCESS, api_name);
    assert_ansi_string(
        &ansi_string,
        wcslen(unicode_text),
        wcslen(unicode_text) + 1,
        ansi_text,
        "Convert partial unicode to ansi string (alloc)."
    );

    if (result == STATUS_SUCCESS) {
        RtlFreeAnsiString(&ansi_string);
    }

    // Initialize our own ansi string.
    ansi_string.Length = wcslen(unicode_text);
    ansi_string.MaximumLength = ansi_string.Length + 1;
    ansi_string.Buffer = ansi_buffer;
    result = RtlUnicodeStringToAnsiString(&ansi_string, &unicode_string, 0);
    assert_NTSTATUS(result, STATUS_SUCCESS, api_name);
    assert_ansi_string(
        &ansi_string,
        wcslen(unicode_text),
        wcslen(unicode_text) + 1,
        ansi_text,
        "Convert partial unicode to ansi string."
    );
    memset(ansi_string.Buffer, 0, ansi_string.MaximumLength);

    // Finally, try modify member variables to get any other result come back as invalid.

    // Increase unicode string by one to trigger buffer overflow status.
    // Yet, at least get a partial returned buffer.
    // Ansi string's max and current lengths should remain the same.
    unicode_string.Length += 2;
    unicode_string.MaximumLength += 2;
    result = RtlUnicodeStringToAnsiString(&ansi_string, &unicode_string, 0);
    assert_NTSTATUS(result, STATUS_BUFFER_OVERFLOW, api_name);
    assert_ansi_string(
        &ansi_string,
        wcslen(unicode_text),
        wcslen(unicode_text) + 1,
        ansi_text,
        "Convert unicode (up by one length) to limited ansi string."
    );
    memset(ansi_string.Buffer, 0, ansi_string.MaximumLength);

    // When default behavior is working, try override to use whole unicode text.
    unicode_string.MaximumLength = sizeof(unicode_text);
    unicode_string.Length = unicode_string.MaximumLength - 1 * sizeof(WCHAR);

    // Since we didn't update ansi string, we should trigger buffer overflow status.
    // Yet, at least get a partial returned buffer.
    result = RtlUnicodeStringToAnsiString(&ansi_string, &unicode_string, 0);
    assert_NTSTATUS(result, STATUS_BUFFER_OVERFLOW, api_name);
    assert_ansi_string(
        &ansi_string,
        wcslen(unicode_text),
        wcslen(unicode_text) + 1,
        ansi_text,
        "Convert max unicode to limited ansi string."
    );
    memset(ansi_string.Buffer, 0, ansi_string.MaximumLength);

    // Now let's update ansi string's length to the max.
    ansi_string.MaximumLength = sizeof(ansi_buffer);
    ansi_string.Length = ansi_string.MaximumLength - 1;
    result = RtlUnicodeStringToAnsiString(&ansi_string, &unicode_string, 0);
    assert_NTSTATUS(result, STATUS_SUCCESS, api_name);
    assert_ansi_string(
        &ansi_string,
        ARRAY_SIZE(unicode_text) - 1,
        ARRAY_SIZE(unicode_text),
        ansi_text,
        "Convert full unicode to ansi string."
    );
    memset(ansi_string.Buffer, 0, ansi_string.MaximumLength);

    ansi_string.MaximumLength = 0;
    result = RtlUnicodeStringToAnsiString(&ansi_string, &unicode_string, 0);
    assert_NTSTATUS(result, STATUS_BUFFER_OVERFLOW, api_name);
    // We can't do ansi string assert check since max length of ansi string is set to 0.

    // Allocate unicode and ansi strings for attempt to maxed out.
    WCHAR* unicode_text_max = ExAllocatePoolWithTag(UINT16_MAX, 'grtS');
    memcpy(unicode_text_max, unicode_text, sizeof(unicode_text));
    char* ansi_text_max = ExAllocatePoolWithTag(UINT16_MAX, 'grtS');
    memcpy(ansi_text_max, ansi_text, sizeof(ansi_text));

    // Increase ansi string length to max.
    ansi_string.MaximumLength = UINT16_MAX;
    ansi_string.Length = UINT16_MAX;
    ansi_string.Buffer = ansi_text_max;
    result = RtlUnicodeStringToAnsiString(&ansi_string, &unicode_string, 0);
    assert_NTSTATUS(result, STATUS_SUCCESS, api_name);
    assert_ansi_string(
        &ansi_string,
        ARRAY_SIZE(unicode_text) - 1,
        UINT16_MAX,
        ansi_text_max,
        "Unicode to max ansi string."
    );
    memset(ansi_string.Buffer, 0, ansi_string.MaximumLength);

    // Now increase unicode string length to max.
    // We can't multiply by size of WCHAR due to uint16_t's max size
    unicode_string.MaximumLength = UINT16_MAX;
    unicode_string.Length = UINT16_MAX;
    unicode_string.Buffer = unicode_text_max;
    result = RtlUnicodeStringToAnsiString(&ansi_string, &unicode_string, 0);
    assert_NTSTATUS(result, STATUS_SUCCESS, api_name);
    assert_ansi_string(
        &ansi_string,
        (UINT16_MAX-1) / sizeof(WCHAR),
        UINT16_MAX,
        ansi_text_max,
        "Max unicode to max ansi string."
    );
    memset(ansi_string.Buffer, 0, ansi_string.MaximumLength);

    // Free up our allocated buffers
    ExFreePool(ansi_text_max);
    ExFreePool(unicode_text_max);

    TEST_END();
}

TEST_FUNC(RtlUnicodeStringToInteger)
{
    TEST_BEGIN();

    UNICODE_STRING str;
    ULONG value;
    NTSTATUS status;

    // Base 10
    RtlInitUnicodeString(&str, L"12345");
    value = 0;
    status = RtlUnicodeStringToInteger(&str, 10, &value);
    GEN_CHECK(status, STATUS_SUCCESS, "base10 status");
    GEN_CHECK(value, 12345, "base10 value");

    // Base 16
    RtlInitUnicodeString(&str, L"FF");
    value = 0;
    status = RtlUnicodeStringToInteger(&str, 16, &value);
    GEN_CHECK(status, STATUS_SUCCESS, "base16 status");
    GEN_CHECK(value, 0xFF, "base16 value");

    // Base 0 with 0x prefix
    RtlInitUnicodeString(&str, L"0x1A");
    value = 0;
    status = RtlUnicodeStringToInteger(&str, 0, &value);
    GEN_CHECK(status, STATUS_SUCCESS, "0x prefix status");
    GEN_CHECK(value, 0x1A, "0x prefix value");

    // Negative number
    RtlInitUnicodeString(&str, L"-100");
    value = 0;
    status = RtlUnicodeStringToInteger(&str, 10, &value);
    GEN_CHECK(status, STATUS_SUCCESS, "negative status");
    GEN_CHECK(value, (ULONG)-100, "negative value");

    // Leading whitespace
    RtlInitUnicodeString(&str, L"  42");
    value = 0;
    status = RtlUnicodeStringToInteger(&str, 10, &value);
    GEN_CHECK(status, STATUS_SUCCESS, "whitespace status");
    GEN_CHECK(value, 42, "whitespace value");

    // Invalid base
    RtlInitUnicodeString(&str, L"1");
    status = RtlUnicodeStringToInteger(&str, 1, &value);
    GEN_CHECK(status, STATUS_INVALID_PARAMETER, "invalid base status");

    TEST_END();
}

TEST_FUNC(RtlUnicodeToMultiByteN)
{
    TEST_BEGIN();

    WCHAR src[] = L"Xbox";
    CHAR dest[16] = { 0 };
    ULONG bytes_written = 0;
    NTSTATUS status;

    // Normal conversion
    status = RtlUnicodeToMultiByteN(dest, sizeof(dest), &bytes_written, src, 8);
    GEN_CHECK(status, STATUS_SUCCESS, "normal status");
    GEN_CHECK(bytes_written, 4, "bytes_written");
    GEN_CHECK(dest[0], 'X', "dest[0]");
    GEN_CHECK(dest[1], 'b', "dest[1]");
    GEN_CHECK(dest[2], 'o', "dest[2]");
    GEN_CHECK(dest[3], 'x', "dest[3]");

    // Limited output buffer
    RtlZeroMemory(dest, sizeof(dest));
    bytes_written = 0;
    status = RtlUnicodeToMultiByteN(dest, 2, &bytes_written, src, 8);
    GEN_CHECK(status, STATUS_BUFFER_OVERFLOW, "limited status");
    GEN_CHECK(bytes_written, 2, "limited bytes_written");
    GEN_CHECK(dest[0], 'X', "limited dest[0]");
    GEN_CHECK(dest[1], 'b', "limited dest[1]");

    // Empty source
    RtlZeroMemory(dest, sizeof(dest));
    bytes_written = 0;
    status = RtlUnicodeToMultiByteN(dest, sizeof(dest), &bytes_written, src, 0);
    GEN_CHECK(status, STATUS_SUCCESS, "empty status");
    GEN_CHECK(bytes_written, 0, "empty bytes_written");

    TEST_END();
}

TEST_FUNC(RtlUnicodeToMultiByteSize)
{
    TEST_BEGIN();

    ULONG multi_size = 0;
    NTSTATUS status;

    // Each unicode char (2 bytes) maps to 1 multibyte char
    status = RtlUnicodeToMultiByteSize(&multi_size, L"Xbox", 8);
    GEN_CHECK(status, STATUS_SUCCESS, "status");
    GEN_CHECK(multi_size, 4, "size for 4 wchars");

    status = RtlUnicodeToMultiByteSize(&multi_size, L"", 0);
    GEN_CHECK(status, STATUS_SUCCESS, "empty status");
    GEN_CHECK(multi_size, 0, "size for 0 wchars");

    status = RtlUnicodeToMultiByteSize(&multi_size, L"A", 2);
    GEN_CHECK(status, STATUS_SUCCESS, "single status");
    GEN_CHECK(multi_size, 1, "size for 1 wchar");

    TEST_END();
}
