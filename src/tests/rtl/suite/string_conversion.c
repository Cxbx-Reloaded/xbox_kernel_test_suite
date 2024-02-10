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

void test_RtlAnsiStringToUnicodeString(int func_num, const char* func_name)
{
    BOOL tests_passed = 1;

    print_test_header(func_num, func_name);

    const uint32_t long_str_size = 0x10000;
    UNICODE_STRING dest_str = { 0 };
    ANSI_STRING src_str = { 0 };
    CHAR* long_str = malloc(sizeof(CHAR) * long_str_size);
    if(long_str == NULL) {
        print("ERROR: Could not malloc long_str");
    }

    NTSTATUS ret = RtlAnsiStringToUnicodeString(&dest_str, &src_str, 0);
    tests_passed &= assert_NTSTATUS(
        ret,
        STATUS_BUFFER_OVERFLOW,
        func_name
    );

    memset(long_str, 'a', long_str_size);
    long_str[long_str_size - 1] = '\0';
    RtlInitAnsiString(&src_str, long_str);
    ret = RtlAnsiStringToUnicodeString(&dest_str, &src_str, 0);
    tests_passed &= assert_NTSTATUS(
        ret,
        STATUS_INVALID_PARAMETER_2,
        func_name
    );
    free(long_str);

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlCharToInteger(int func_num, const char* func_name)
{
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    // if base != (2 | 8 | 10 | 16) return STATUS_INVALID_PARAMETER
    // SPECIAL CASE: Base = 0 will not throw this error as base = 0 signifies that the base will be included
    // in the input string: '0b' = binary, '0o' = octal, '0x' = hex
    NTSTATUS ret = RtlCharToInteger("1", 1, NULL);
    test_passed &= assert_NTSTATUS(ret, STATUS_INVALID_PARAMETER, func_name);

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

    print_test_footer(func_num, func_name, test_passed);
}

void test_RtlIntegerToChar(int func_num, const char* func_name)
{
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlIntegerToUnicodeString(int func_num, const char* func_name)
{
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlMultiByteToUnicodeN(int func_num, const char* func_name)
{
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlMultiByteToUnicodeSize(int func_num, const char* func_name)
{
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUnicodeStringToAnsiString(int func_num, const char* func_name)
{
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    UNICODE_STRING unicode_string;
    WCHAR unicode_text[] = L"Xbox\x0100\xFFFF\0Xbox\x0255";
    char ansi_text[] = "Xbox??\0Xbox?";
    char ansi_buffer[sizeof(ansi_text)];
    ANSI_STRING ansi_string;
    const BOOL alloc_buffer = 1;

    // Test if default behavior is working as intended.
    RtlInitUnicodeString(&unicode_string, unicode_text);
    tests_passed &= assert_unicode_string(
        &unicode_string,
        wcslen(unicode_text) * sizeof(WCHAR),
        (wcslen(unicode_text) + 1) * sizeof(WCHAR),
        unicode_text,
        "Initialize unicode string."
    );

    // Test default behavior for allocated buffer.
    NTSTATUS result = RtlUnicodeStringToAnsiString(&ansi_string, &unicode_string, alloc_buffer);
    tests_passed &= assert_NTSTATUS(result, STATUS_SUCCESS, func_name);
    tests_passed &= assert_ansi_string(
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
    tests_passed &= assert_NTSTATUS(result, STATUS_SUCCESS, func_name);
    tests_passed &= assert_ansi_string(
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
    tests_passed &= assert_NTSTATUS(result, STATUS_BUFFER_OVERFLOW, func_name);
    tests_passed &= assert_ansi_string(
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
    tests_passed &= assert_NTSTATUS(result, STATUS_BUFFER_OVERFLOW, func_name);
    tests_passed &= assert_ansi_string(
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
    tests_passed &= assert_NTSTATUS(result, STATUS_SUCCESS, func_name);
    tests_passed &= assert_ansi_string(
        &ansi_string,
        ARRAY_SIZE(unicode_text) - 1,
        ARRAY_SIZE(unicode_text),
        ansi_text,
        "Convert full unicode to ansi string."
    );
    memset(ansi_string.Buffer, 0, ansi_string.MaximumLength);

    ansi_string.MaximumLength = 0;
    result = RtlUnicodeStringToAnsiString(&ansi_string, &unicode_string, 0);
    tests_passed &= assert_NTSTATUS(result, STATUS_BUFFER_OVERFLOW, func_name);
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
    tests_passed &= assert_NTSTATUS(result, STATUS_SUCCESS, func_name);
    tests_passed &= assert_ansi_string(
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
    tests_passed &= assert_NTSTATUS(result, STATUS_SUCCESS, func_name);
    tests_passed &= assert_ansi_string(
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

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlUnicodeStringToInteger(int func_num, const char* func_name)
{
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUnicodeToMultiByteN(int func_num, const char* func_name)
{
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUnicodeToMultiByteSize(int func_num, const char* func_name)
{
    /* FIXME: This is a stub! implement this function! */
}
