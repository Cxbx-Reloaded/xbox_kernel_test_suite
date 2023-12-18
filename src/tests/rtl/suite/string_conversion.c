#include <xboxkrnl/xboxkrnl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/common.h"
#include "assertions/rtl.h"

void test_RtlAnsiStringToUnicodeString()
{
    const char* func_num = "0x0104";
    const char* func_name = "RtlAnsiStringToUnicodeString";
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

static BOOL check_RtlCharToInteger_result(
    CHAR* input,
    ULONG base,
    ULONG expected_result,
    ULONG result,
    NTSTATUS retval)
{
    if((retval == STATUS_SUCCESS) && (result == expected_result)) {
        print("  Test PASSED for input = %s, base = %u, expected_result = %d", input, base, expected_result);
        return 1;
    }
    print("  Test FAILED for input = %s, base = %u, expected_result = %d, result = %d, NTSTATUS = 0x%x",
            input, base, expected_result, result, retval);
    return 0;
}

void test_RtlCharToInteger()
{
    const char* func_num = "0x010B";
    const char* func_name = "RtlCharToInteger";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    // if base != (2 | 8 | 10 | 16) return STATUS_INVALID_PARAMETER
    // SPECIAL CASE: Base = 0 will not throw this error as base = 0 signifies that the base will be included
    // in the input string: '0b' = binary, '0o' = octal, '0x' = hex
    NTSTATUS ret = RtlCharToInteger("1", 1, NULL);
    tests_passed &= assert_NTSTATUS(ret, STATUS_INVALID_PARAMETER, func_name);

    // Test all of the different bases with positive and negative results, including base = 0.
    // In test cases where there are invalid numbers for the specified base, the RtlCharToInteger will
    // convert the valid numbers up to invalid number.
    // For example, base = 2, input = '1015C' will return 0x5
    CHAR* inputs[]           = {"11001010", "7631", "1100", "5FAC2", "101C813", "76BA787", "1000B1"};
    CHAR* base_formats[]     = {"0b"      , "0o"  , ""    , "0x"   , "0b"     , "0o"     , ""      };
    ULONG base[]             = {2         , 8     , 10    , 16     , 2        , 8        , 10      };
    ULONG expected_results[] = {0xCA      , 0xF99 , 1100  , 0x5FAC2, 0x5      , 0x3E     , 1000    };

    NTSTATUS base_ret, neg_base_ret, format_ret, neg_format_ret;
    ULONG    base_result, neg_base_result, format_result, neg_format_result, neg_expected_result;
    CHAR     neg_base_buffer[50], format_buffer[50], neg_format_buffer[50];
    for(uint8_t i = 0; i < ARRAY_SIZE(expected_results); i++) {
        strcpy(neg_base_buffer, "-");
        strcat(neg_base_buffer, inputs[i]);

        strcpy(format_buffer, base_formats[i]);
        strcat(format_buffer, inputs[i]);

        strcpy(neg_format_buffer, "-");
        strcat(neg_format_buffer, base_formats[i]);
        strcat(neg_format_buffer, inputs[i]);

        base_ret        = RtlCharToInteger(inputs[i]        , base[i], &base_result);
        neg_base_ret    = RtlCharToInteger(neg_base_buffer  , base[i], &neg_base_result);
        format_ret      = RtlCharToInteger(format_buffer    , 0      , &format_result);
        neg_format_ret  = RtlCharToInteger(neg_format_buffer, 0      , &neg_format_result);

        neg_expected_result = -1 * expected_results[i];
        tests_passed &= check_RtlCharToInteger_result(inputs[i]        , base[i], expected_results[i], base_result      , base_ret);
        tests_passed &= check_RtlCharToInteger_result(neg_base_buffer  , base[i], neg_expected_result, neg_base_result  , neg_base_ret);
        tests_passed &= check_RtlCharToInteger_result(format_buffer    , 0      , expected_results[i], format_result    , format_ret);
        tests_passed &= check_RtlCharToInteger_result(neg_format_buffer, 0      , neg_expected_result, neg_format_result, neg_format_ret);
    }

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlIntegerToChar()
{
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlIntegerToUnicodeString()
{
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlMultiByteToUnicodeN()
{
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlMultiByteToUnicodeSize()
{
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUnicodeStringToAnsiString()
{
    const char* func_num = "0x0134";
    const char* func_name = "RtlUnicodeStringToAnsiString";
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

void test_RtlUnicodeStringToInteger()
{
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUnicodeToMultiByteN()
{
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUnicodeToMultiByteSize()
{
    /* FIXME: This is a stub! implement this function! */
}
