#include <xboxkrnl/xboxkrnl.h>
#include <hal/xbox.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <wchar.h>
#include <windows.h>

#include "global.h"
#include "util/output.h"
#include "assertions/common.h"
#include "assertions/rtl.h"
#include "assertions/ke.h"
#include "assertions/defines.h"

const char* failed_text = "FAILED";
const char* passed_text = "PASSED";

void test_RtlAnsiStringToUnicodeString(){
    const char* func_num = "0x0104";
    const char* func_name = "RtlAnsiStringToUnicodeString";
    BOOL tests_passed = 1;

    print_test_header(func_num, func_name);

    const uint32_t long_str_size = 0x10000;
    UNICODE_STRING dest_str;
    ANSI_STRING src_str;
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

// Odd behavior of this function: A terminating NULL character '\0' is not added to the end of the appended string.
void test_RtlAppendStringToString(){
    const char* func_num = "0x0105";
    const char* func_name = "RtlAppendStringToString";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

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
    tests_passed &= assert_NTSTATUS(ret, STATUS_SUCCESS, func_name);
    dest_buffer[dest_str.Length] = '\0'; // RtlAppendStringToString does not add a terminating NULL character which breaks strncmp
    tests_passed &= assert_ansi_string(
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
    tests_passed &= assert_NTSTATUS(ret, STATUS_SUCCESS, func_name);
    tests_passed &= assert_ansi_string(
        &dest_str,
        strlen(expected_result),
        sizeof(dest_buffer),
        expected_result,
        "Append src_str to dest_str a second time"
    );

    // Should now return STATUS_BUFFER_TOO_SMALL
    RtlInitAnsiString(&src_str, "A");
    ret = RtlAppendStringToString(&dest_str, &src_str);
    tests_passed &= assert_NTSTATUS(ret, STATUS_BUFFER_TOO_SMALL, func_name);

    // Appending empty str is okay though
    RtlInitAnsiString(&src_str, "");
    ret = RtlAppendStringToString(&dest_str, &src_str);
    tests_passed &= assert_NTSTATUS(ret, STATUS_SUCCESS, func_name);
    tests_passed &= assert_ansi_string(
        &dest_str,
        strlen(expected_result),
        sizeof(dest_buffer),
        expected_result,
        "Appending empty_str does not change dest_str"
    );

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlAppendUnicodeStringToString(){
    const char* func_num = "0x0106";
    const char* func_name = "RtlAppendUnicodeStringToString";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

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
    tests_passed &= assert_NTSTATUS(ret, STATUS_SUCCESS, func_name);
    tests_passed &= assert_unicode_string(
        &dest_str,
        wcslen(expected_result) * sizeof(WCHAR),
        num_buf_bytes,
        expected_result,
        "Append src str to empty dest str"
    );

    wcscat(expected_result, src_str.Buffer);
    ret = RtlAppendUnicodeStringToString(&dest_str, &src_str);
    tests_passed &= assert_NTSTATUS(ret, STATUS_SUCCESS, func_name);
    tests_passed &= assert_unicode_string(
        &dest_str,
        wcslen(expected_result) * sizeof(WCHAR),
        num_buf_bytes,
        expected_result,
        "Append src str to dest str again"
    );

    ret = RtlAppendUnicodeStringToString(&dest_str, &src_str);
    tests_passed &= assert_NTSTATUS(ret, STATUS_BUFFER_TOO_SMALL, func_name);

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlAppendUnicodeToString(){
    const char* func_num = "0x0107";
    const char* func_name = "RtlAppendUnicodeToString";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const WCHAR src_text[] = L"Xbox";
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
    tests_passed &= assert_NTSTATUS(ret, STATUS_SUCCESS, func_name);
    tests_passed &= assert_unicode_string(
        &dest_str,
        wcslen(expected_result) * sizeof(WCHAR),
        num_buf_bytes,
        expected_result,
        "Append src str to empty dest str"
    );

    wcsncat(expected_result, src_text, num_chars_in_src);
    ret = RtlAppendUnicodeToString(&dest_str, src_text);
    tests_passed &= assert_NTSTATUS(ret, STATUS_SUCCESS, func_name);
    tests_passed &= assert_unicode_string(
        &dest_str,
        wcslen(expected_result) * sizeof(WCHAR),
        num_buf_bytes,
        expected_result,
        "Append src str to dest str again"
    );

    ret = RtlAppendUnicodeToString(&dest_str, src_text);
    tests_passed &= assert_NTSTATUS(ret, STATUS_BUFFER_TOO_SMALL, func_name);

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlAssert(){
    /* FIXME: This is a stub! implement this function! */
}

// Notes about this function: The official kernel allocates 0x440 bytes for the CONTEXT structure.
// That does not match with any other implementation of CONTEXT that I have been able to find.
// RtlCaptureContext does not touch the ContextFlags or Floating point save fields of the CONTEXT
// structure.
//
// FIXME: RtlCaptureContext also sets Ebp, Eip, and Esp in the context record. No checks are enabled for them
// currently.
void test_RtlCaptureContext(){
    const char* func_num = "0x0109";
    const char* func_name = "RtlCaptureContext";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    CONTEXT result_context;
    // cs and ss words so make sure that the upper word of the Seg* dword is 0 or the tests will fail
    // due to unitialized values.
    result_context.SegCs = 0;
    result_context.SegSs = 0;

    DWORD eax_val = 0x336699CC;
    DWORD ebx_val = 0x11223344;
    DWORD ecx_val = 0x55667788;
    DWORD edx_val = 0x99AABBCC;
    DWORD esi_val = 0xDDEEFF11;
    DWORD edi_val = 0xDEADBEEF;
    DWORD flags_val = 0;
    // Again, cs and ss are words so make sure the upper word is initialized to 0
    DWORD cs_val = 0;
    DWORD ss_val = 0;
    DWORD RtlCaptureContext_address = 0;

    asm volatile(
        // Make sure we can clobber all registers
        "pusha\n"
        // Get address of RtlCaptureContext and store it for later
        "movl __imp__RtlCaptureContext@4, %%eax\n"
        "movl %%eax, %0\n"
        // Push &result_context to pass into RtlCaptureContext
        "leal %1, %%eax\n"
        "pushl %%eax\n"
        // Store current values of cs and ss
        "leal %2, %%eax\n"
        "movw %%cs, (%%eax)\n"
        "leal %3, %%eax\n"
        "movw %%ss, (%%eax)\n"
        // Store current value of the flags
        "leal %4, %%eax\n"
        "pushf\n"
        "popl (%%eax)\n"
        // Setup pre-set register values
        "movl %5, %%eax\n"
        "movl %6, %%ebx\n"
        "movl %7, %%ecx\n"
        "movl %8, %%edx\n"
        "movl %9, %%esi\n"
        "movl %10, %%edi\n"
        // Call RtlCaptureContext and cleanup registers
        "calll *%11\n"
        "popa\n"
        : "=m" (RtlCaptureContext_address)
        : "m" (result_context), "m" (cs_val), "m" (ss_val), "m" (flags_val),
          "m" (eax_val), "m" (ebx_val), "m" (ecx_val), "m" (edx_val), "m" (esi_val),
          "m" (edi_val), "m" (RtlCaptureContext_address)
    );

    DWORD* expected_vals[]  = {&eax_val, &ebx_val, &ecx_val, &edx_val, &esi_val, &edi_val,
                               &cs_val, &ss_val, &flags_val};
    DWORD* result_vals[]    = {&result_context.Eax, &result_context.Ebx, &result_context.Ecx,
                               &result_context.Edx, &result_context.Esi, &result_context.Edi,
                               &result_context.SegCs, &result_context.SegSs, &result_context.EFlags};
    const char* reg_names[] = {"Eax", "Ebx", "Ecx", "Edx", "Esi", "Edi", "SegCs", "SegSs", "EFlags"};

    for(uint8_t i = 0; i < sizeof(expected_vals) / sizeof(DWORD*); i++) {
        const char* result_text = passed_text;
        if(*result_vals[i] != *expected_vals[i]) {
            tests_passed = 0;
            result_text = failed_text;
        }
        print("  Test %s Expected %s = 0x%x, got = 0x%x", result_text, reg_names[i], *expected_vals[i], *result_vals[i]);
    }

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlCaptureStackBackTrace(){
    const char* func_num = "0x010A";
    const char* func_name = "RtlCaptureStackBackTrace";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    ULONG frame_size = 10;
    PULONG frames_expected = (PULONG)malloc(frame_size * sizeof(ULONG));
    PULONG frames_test = (PULONG)malloc(frame_size * sizeof(ULONG));

    // First, we need to collect frames_expected to perform later tests.
    frame_size = RtlWalkFrameChain((PVOID*)frames_expected, frame_size, 0);
    if (!frame_size) {
        // If we are not getting any return size, force fail.
        GEN_CHECK(frame_size, 1, "frame_size");
    }
    else {
        // Check all frames capture without any skip.
        ULONG frames_skip = 1; // We want to skip at least one due to the fact it point to first call within kernel.
        ULONG frames_capture = frame_size - 1;

        ULONG frames_hash_expected = 0, frames_hash_test = 0;
        USHORT frames_count = RtlCaptureStackBackTrace(frames_skip, frames_capture, (PVOID*)frames_test, &frames_hash_test);

        GEN_CHECK(frames_count, frames_capture, "frames_count1");

        PULONG frames_expected_offset = frames_expected + frames_skip;
        GEN_CHECK_ARRAY(frames_test, frames_expected_offset, frames_count, "frames1");

        for (unsigned i = frames_skip; i < frames_skip + frames_count; i++) {
            frames_hash_expected += frames_expected[i];
        }
        GEN_CHECK(frames_hash_test, frames_hash_expected, "frames_hash1");

        // Choose specific portion of the frames we want to check against.
        frames_skip = frame_size / 2;
        frames_capture = frame_size / 2;

        frames_hash_expected = frames_hash_test = 0;
        frames_count = RtlCaptureStackBackTrace(frames_skip, frames_capture, (PVOID*)frames_test, &frames_hash_test);

        GEN_CHECK(frames_count, frames_capture, "frames_count2");

        frames_expected_offset = frames_expected + frames_skip;
        GEN_CHECK_ARRAY(frames_test, frames_expected_offset, frames_count, "frames2");

        for (unsigned i = frames_skip; i < frames_skip + frames_count; i++) {
            frames_hash_expected += frames_expected[i];
        }
        GEN_CHECK(frames_hash_test, frames_hash_expected, "frames_hash2");

        // Choose specific portion of the frames we want to check against.
        frames_skip = frame_size / 2;
        frames_capture = frames_skip / 2;

        frames_hash_expected = frames_hash_test = 0;
        frames_count = RtlCaptureStackBackTrace(frames_skip, frames_capture, (PVOID*)frames_test, &frames_hash_test);

        GEN_CHECK(frames_count, frames_capture, "frames_count3");

        frames_expected_offset = frames_expected + frames_skip;
        GEN_CHECK_ARRAY(frames_test, frames_expected_offset, frames_count, "frames3");

        for (unsigned i = frames_skip; i < frames_skip + frames_count; i++) {
            frames_hash_expected += frames_expected[i];
        }
        GEN_CHECK(frames_hash_test, frames_hash_expected, "frames_hash3");
    }

    free(frames_expected);
    free(frames_test);

    print_test_footer(func_num, func_name, test_passed);
}

static BOOL check_RtlCharToInteger_result(
    CHAR* input,
    ULONG base,
    ULONG expected_result,
    ULONG result,
    NTSTATUS retval
) {
    if((retval == STATUS_SUCCESS) && (result == expected_result)) {
        print("  Test PASSED for input = %s, base = %u, expected_result = %d", input, base, expected_result);
        return 1;
    }
    print("  Test FAILED for input = %s, base = %u, expected_result = %d, result = %d, NTSTATUS = 0x%x",
            input, base, expected_result, result, retval);
    return 0;
}

void test_RtlCharToInteger(){
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
    for(uint8_t i = 0; i < sizeof(expected_results) / sizeof(ULONG); i++) {
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

void test_RtlCompareMemory(){
    const char* func_num = "0x010C";
    const char* func_name = "RtlCompareMemory";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    uint32_t num1 = 0;
    uint32_t num2 = 0;
    VOID* num1_ptr = (VOID*)&num1;
    VOID* num2_ptr = (VOID*)&num2;

    SIZE_T bytes_matching = RtlCompareMemory(num1_ptr, num2_ptr, 0);
    tests_passed &= assert_rtl_compared_bytes(bytes_matching, 0, "Check 0 bytes of memory");

    bytes_matching = RtlCompareMemory(num1_ptr, num2_ptr, sizeof(num1));
    tests_passed &= assert_rtl_compared_bytes(bytes_matching, sizeof(num1), "Both nums 0, should match");

    num1 |= 0x80000000;
    bytes_matching = RtlCompareMemory(num1_ptr, num2_ptr, sizeof(num1));
    tests_passed &= assert_rtl_compared_bytes(bytes_matching, sizeof(num1) - 1, "Only 3 bytes should match");

    num2 |= 0x4000;
    bytes_matching = RtlCompareMemory(num1_ptr, num2_ptr, sizeof(num1));
    tests_passed &= assert_rtl_compared_bytes(bytes_matching, sizeof(num1) - 3, "Only 1 byte should match");

    num2 |= 0x80000000;
    num1 |= 0x4000;
    bytes_matching = RtlCompareMemory(num1_ptr, num2_ptr, sizeof(num1));
    tests_passed &= assert_rtl_compared_bytes(bytes_matching, sizeof(num1), "All bytes should match again");

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlCompareMemoryUlong(){
    const char* func_num = "0x010D";
    const char* func_name = "RtlCompareMemoryUlong";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    ULONG mem[4];
    ULONG res;
    mem[0]= 0x0123;
    mem[1]= 0x4567;
    mem[2]= 0x89ab;
    mem[3]= 0xcdef;

    tests_passed &= RtlCompareMemoryUlong(mem, 0, 0x0123) == 0 ? 1 : 0;
    tests_passed &= RtlCompareMemoryUlong(mem, 3, 0x0123) == 0 ? 1 : 0;
    tests_passed &= RtlCompareMemoryUlong(mem, 4, 0x0123) == 4 ? 1 : 0;
    tests_passed &= RtlCompareMemoryUlong(mem, 5, 0x0123) == 4 ? 1 : 0;
    tests_passed &= RtlCompareMemoryUlong(mem, 7, 0x0123) == 4 ? 1 : 0;
    tests_passed &= RtlCompareMemoryUlong(mem, 8, 0x0123) == 4 ? 1 : 0;
    tests_passed &= RtlCompareMemoryUlong(mem, 9, 0x0123) == 4 ? 1 : 0;
    tests_passed &= RtlCompareMemoryUlong(mem, 4, 0x0127) == 0 ? 1 : 0;
    tests_passed &= RtlCompareMemoryUlong(mem, 4, 0x7123) == 0 ? 1 : 0;
    tests_passed &= RtlCompareMemoryUlong(mem, 16, 0x4567) == 0 ? 1 : 0;

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlCompareString(){
    const char* func_num = "0x010E";
    const char* func_name = "RtlCompareString";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    ANSI_STRING lower_case_string;
    ANSI_STRING upper_case_string;
    ANSI_STRING long_string;

    RtlInitAnsiString(&lower_case_string, "test_string");
    RtlInitAnsiString(&upper_case_string, "TEST_STRING");
    RtlInitAnsiString(&long_string, "this_is_a_long_string");

    tests_passed &= RtlCompareString(&lower_case_string, &upper_case_string, 1) == 0 ? 1 : 0;
    tests_passed &= RtlCompareString(&lower_case_string, &upper_case_string, 0) != 0 ? 1 : 0;
    tests_passed &= RtlCompareString(&lower_case_string, &lower_case_string, 1) == 0 ? 1 : 0;
    tests_passed &= RtlCompareString(&lower_case_string, &lower_case_string, 0) == 0 ? 1 : 0;
    tests_passed &= RtlCompareString(&lower_case_string, &long_string, 1) < 0 ? 1 : 0;
    tests_passed &= RtlCompareString(&long_string, &lower_case_string, 1) > 0 ? 1 : 0;

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlCompareUnicodeString(){
    const char* func_num = "0x010F";
    const char* func_name = "RtlCompareUnicodeString";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const BOOL case_sensitive = 0;
    const BOOL case_insensitive = 1;
    UNICODE_STRING base_str, lowercase_base_str, shorter_str, longer_str;
    LONG result;

    RtlInitUnicodeString(&base_str, L"Xbox");
    RtlInitUnicodeString(&lowercase_base_str, L"xbox");
    RtlInitUnicodeString(&shorter_str, L"Xb");
    RtlInitUnicodeString(&longer_str, L"Xbox Original");

    result = RtlCompareUnicodeString(&base_str, &base_str, case_sensitive);
    result += RtlCompareUnicodeString(&base_str, &base_str, case_insensitive);
    if(result != 0) {
        print("  Comparing a string to itself failed with result: %d", result);
        tests_passed = 0;
    }

    result = RtlCompareUnicodeString(&base_str, &lowercase_base_str, case_sensitive);
    if( !(result < 0) ) {
        print("  Case sensitive compare of case mismatching strings failed with result %d", result);
        tests_passed = 0;
    }
    result = RtlCompareUnicodeString(&base_str, &lowercase_base_str, case_insensitive);
    if(result != 0) {
        print("  Case insensitive compare of case mismatching strings failed with result %d", result);
        tests_passed = 0;
    }

    result = RtlCompareUnicodeString(&base_str, &shorter_str, case_sensitive);
    if( !(result > 0) ) {
        print("  Case sensitive compare of shorter string failed with result %d", result);
        tests_passed = 0;
    }
    result = RtlCompareUnicodeString(&base_str, &shorter_str, case_insensitive);
    if( !(result > 0) ) {
        print("  Case insensitive compare of shorter string failed with result %d", result);
        tests_passed = 0;
    }

    result = RtlCompareUnicodeString(&base_str, &longer_str, case_sensitive);
    if( !(result < 0) ) {
        print("  Case sensitive compare of longer string failed with result %d", result);
        tests_passed = 0;
    }
    result = RtlCompareUnicodeString(&base_str, &longer_str, case_insensitive);
    if( !(result < 0) ) {
        print("  Case insensitive compare of longer string failed with result %d", result);
        tests_passed = 0;
    }

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlCopyString(){
    const char* func_num = "0x0110";
    const char* func_name = "RtlCopyString";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    char src_char_arr[] = "xbox is cool";
    char dst_char_arr[] = "            ";
    STRING src_str;
    STRING dst_str;

    RtlInitAnsiString(&src_str, src_char_arr);
    RtlInitAnsiString(&dst_str, dst_char_arr);
    RtlCopyString(&dst_str, &src_str);

    if(strncmp(src_str.Buffer, dst_char_arr, src_str.Length) != 0)
        tests_passed = 0;

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlCopyUnicodeString(){
    const char* func_num = "0x0111";
    const char* func_name = "RtlCopyUnicodeString";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    UNICODE_STRING dest_str, shorter_str, longer_str;
    WCHAR buffer[8];
    const uint8_t num_buf_bytes = sizeof(buffer);

    RtlInitUnicodeString(&shorter_str, L"Xbox");
    RtlInitUnicodeString(&longer_str, L"Xbox Original");
    dest_str.Length = 8;
    dest_str.MaximumLength = num_buf_bytes;
    dest_str.Buffer = buffer;

    RtlCopyUnicodeString(&dest_str, NULL);
    tests_passed &= assert_unicode_string(&dest_str, 0, num_buf_bytes, buffer, "NULL src_str");

    dest_str.Length = 0;
    RtlCopyUnicodeString(&dest_str, &shorter_str);
    tests_passed &= assert_unicode_string(
        &dest_str,
        shorter_str.Length,
        num_buf_bytes,
        buffer,
        "Copy shorter str into str"
    );

    dest_str.Length = 0;
    RtlCopyUnicodeString(&dest_str, &longer_str);
    tests_passed &= assert_unicode_string(
        &dest_str,
        num_buf_bytes,
        num_buf_bytes,
        buffer,
        "Copy longer str into str"
    );

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlCreateUnicodeString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlDowncaseUnicodeChar(){
    const char* func_num = "0x0113";
    const char* func_name = "RtlDowncaseUnicodeChar";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    WCHAR result;
    WCHAR input[]           = {L' ', L'w', L'W', L'X', L']', L'$'};
    WCHAR expected_output[] = {L' ', L'w', L'w', L'x', L']', L'$'};

    for(uint8_t i = 0; i < sizeof(input) / sizeof(WCHAR); i++) {
        const char* result_text = passed_text;
        result = RtlDowncaseUnicodeChar(input[i]);
        if(result != expected_output[i]) {
            tests_passed = 0;
            result_text = failed_text;
        }
        // Change %u to %S when printf supports %S in nxdk
        print("  Test %s for input #%u", result_text, i);
    }

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlDowncaseUnicodeString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlEnterCriticalSection(){
    const char* func_num = "0x0115";
    const char* func_name = "RtlEnterCriticalSection";
    RTL_CRITICAL_SECTION crit_section;
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    RtlInitializeCriticalSection(&crit_section);
    RtlEnterCriticalSection(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        0,
        1,
        (HANDLE)KeGetCurrentThread(),
        "Enter an unused critical section"
    );

    RtlEnterCriticalSection(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        1,
        2,
        (HANDLE)KeGetCurrentThread(),
        "Enter the critical section again"
    );

    RtlLeaveCriticalSection(&crit_section);
    RtlLeaveCriticalSection(&crit_section);
    RtlEnterCriticalSection(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        0,
        1,
        (HANDLE)KeGetCurrentThread(),
        "Re-Enter critical section after leaving twice"
    );

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlEnterCriticalSectionAndRegion(){
    const char* func_num = "0x0116";
    const char* func_name = "RtlEnterCriticalSectionAndRegion";
    RTL_CRITICAL_SECTION crit_section;
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    PKTHREAD thread = KeGetCurrentThread();
    const ULONG orig_APC_disable = thread->KernelApcDisable;

    RtlInitializeCriticalSection(&crit_section);
    RtlEnterCriticalSectionAndRegion(&crit_section);
    RtlEnterCriticalSectionAndRegion(&crit_section);

    tests_passed &= assert_critical_section_equals(
        &crit_section,
        1,
        2,
        (HANDLE)thread,
        "Enter critcal section and region twice"
    );
    tests_passed &= assert_critical_region(thread, orig_APC_disable - 2, "Entered critical region twice");

    // Cleanup machine state without using RtlLeaveCriticalSectionAndRegion because it only
    // increments KernelApcDisable when the critical region is freed (RecursionCount == 0)
    KeLeaveCriticalRegion();
    KeLeaveCriticalRegion();

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlEqualString(){
    const char* func_num = "0x0117";
    const char* func_name = "RtlEqualString";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    ANSI_STRING str1, str2, str3;
    RtlInitAnsiString(&str1, "xbox is cool");
    RtlInitAnsiString(&str2, "XBOX IS COOL");
    RtlInitAnsiString(&str3, "is xbox cool?");

    ANSI_STRING* str1_inputs[]      = {&str1, &str1, &str1, &str1, &str1, &str1};
    ANSI_STRING* str2_inputs[]      = {&str1, &str1, &str2, &str2, &str3, &str3};
    BOOLEAN      case_insensitive[] = {0    , 1    , 0    , 1    , 0    , 1    };
    BOOLEAN      expected_result[]  = {1    , 1    , 0    , 1    , 0    , 0    };

    BOOLEAN result;
    for(uint8_t i = 0; i < sizeof(str1_inputs) / sizeof(ANSI_STRING*); i++) {
        const char* result_text = passed_text;
        result = RtlEqualString(str1_inputs[i], str2_inputs[i], case_insensitive[i]);
        if(result != expected_result[i]) {
            tests_passed = 0;
            result_text = failed_text;
        }
        print("  Test %s for str1 = %s, str2 = %s, case insensitive = %x. Expected = %x, got = %x",
              result_text, str1_inputs[i]->Buffer, str2_inputs[i]->Buffer, case_insensitive[i], expected_result[i],
              result);
    }

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlEqualUnicodeString(){
    const char* func_num = "0x0118";
    const char* func_name = "RtlEqualUnicodeString";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    UNICODE_STRING str1;
    UNICODE_STRING str2;
    UNICODE_STRING str3;

    RtlInitUnicodeString(&str1, L"xbox is cool");
    RtlInitUnicodeString(&str2, L"XBOX IS COOL");
    RtlInitUnicodeString(&str3, L"is xbox cool?");

    tests_passed &= !RtlEqualUnicodeString(&str1, &str2, 0);
    tests_passed &= RtlEqualUnicodeString(&str1, &str2, 1);
    tests_passed &= !RtlEqualUnicodeString(&str1, &str3, 1);
    tests_passed &= !RtlEqualUnicodeString(&str1, &str3, 0);

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlExtendedIntegerMultiply(){
    const char* func_num = "0x0119";
    const char* func_name = "RtlExtendedIntegerMultiply";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const LONGLONG multiplicands[]    = {1,  0,  15,   15, -15};
    const LONG multipliers[]          = {0, -1,  15,  -15, -15};
    const LONGLONG expected_results[] = {0,  0, 225, -225, 225};
    LARGE_INTEGER multiplicand, result;

    for(uint8_t i = 0; i < sizeof(expected_results) / sizeof(LONGLONG); i++) {
        const char* result_text = passed_text;
        multiplicand.QuadPart = multiplicands[i];
        result = RtlExtendedIntegerMultiply(multiplicand, multipliers[i]);
        if(result.QuadPart != expected_results[i]) {
            tests_passed = 0;
            result_text = failed_text;
        }
        print("  Test %s: Expected = %d for multiplicand = %d and multipler = %d, result = %d",
              result_text, (LONG)expected_results[i], (LONG)multiplicands[i], (LONG)multipliers[i], (LONG)result.QuadPart);
    }
    print_test_footer(func_num, func_name, tests_passed);
}

// A few things to note about this function:
// 1. Passing NULL into the remainder field is legal.
// 2. If the divisor = 0, then a STATUS_INTEGER_DIVIDE_BY_ZERO exception will be raised. There is no good way
//    to test this as it will cause real hardware to hang.
// 3. Real hardware implements this as a binary divison algorithm that emits interesting results when negative
//    inputs are provided. The expected_results below are taken from what real hardware produces as output.
void test_RtlExtendedLargeIntegerDivide(){
    const char* func_num = "0x011A";
    const char* func_name = "RtlExtendedLargeIntegerDivide";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const LONGLONG dividends[]        = {1, 5, 300,                -300, 300,        -300};
    const ULONG divisors[]            = {1, 8,   8,                   8,  -8,          -8};
    const ULONG expected_remainders[] = {0, 5,   4,                 0x4, 300,        -244};
    const LONGLONG expected_results[] = {1, 0,  37,  0x1fffffffffffffda,   0, 0x100000007};
    LARGE_INTEGER dividend, result, e_result;
    ULONG remainder;

    for(uint8_t i = 0; i < sizeof(expected_results) / sizeof(LONGLONG); i++) {
        const char* result_text = passed_text;
        dividend.QuadPart = dividends[i];
        e_result.QuadPart = expected_results[i];
        result = RtlExtendedLargeIntegerDivide(dividend, divisors[i], &remainder);
        if((result.QuadPart != e_result.QuadPart) || (remainder != expected_remainders[i])) {
            tests_passed = 0;
            result_text = failed_text;
        }
        // Dec 19th, 2018 - nxdk's printf does not support any format that will print 64 bits.
        // Split the print into the high dword and low dword until nxdk can print them combined.
        print("  Test %s: Expected Result = 0x%x %x with remainder = 0x%x for dividend = 0x%x %x and divisor = 0x%x, result = 0x%x %x with remainder = 0x%x",
              result_text, e_result.u.HighPart, e_result.u.LowPart, expected_remainders[i],
              dividend.u.HighPart, dividend.u.LowPart, divisors[i], result.u.HighPart, result.u.LowPart, remainder);
    }

    // Test remainder = NULL
    dividend.QuadPart = 5;
    result = RtlExtendedLargeIntegerDivide(dividend, 2, NULL);
    if(result.QuadPart != 2) {
        tests_passed = 0;
        print("  Test with NULL remainder FAILED. Expected result = 0x2, got = 0x%x %x",
              result.u.HighPart, result.u.LowPart);
    }
    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlExtendedMagicDivide(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlFillMemory(){
    const char* func_num = "0x011C";
    const char* func_name = "RtlFillMemory";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const BYTE buf_len = 20;
    BYTE buffer[buf_len];
    DWORD lengths[] = {1, 10, buf_len};
    BYTE fills[] = {0x1, 0x2A, 0xFF};
    BOOL individual_test_passes = 1;

    RtlZeroMemory(buffer, buf_len);
    for(uint8_t i = 0; i < sizeof(lengths) / sizeof(DWORD); i++) {
        const char* result_text = passed_text;
        RtlFillMemory(buffer, lengths[i], fills[i]);
        for(uint8_t y = 0; y < buf_len; y++) {
            BYTE expected_fill = (y < lengths[i]) ? fills[i] : 0x00;
            if(buffer[y] != expected_fill) {
                print("  ERROR: For index = %u, got = 0x%x, expected = 0x%x", y, buffer[y], expected_fill);
                individual_test_passes = 0;
            }
        }
        if(!individual_test_passes) {
            tests_passed = 0;
            result_text = failed_text;
        }
        print("  Test %s for length = %u, fill = 0x%x", result_text, lengths[i], fills[i]);
        individual_test_passes = 1;
    }

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlFillMemoryUlong(){
    const char* func_num = "0x011D";
    const char* func_name = "RtlFillMemoryUlong";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const BYTE buf_len = 20;
    ULONG buffer[buf_len];
    SIZE_T lengths[] = {1, 10, buf_len};
    ULONG patterns[] = {0x1, 0x2FFF, 0xFFFFFFFF};
    BOOL individual_test_passes = 1;

    RtlZeroMemory(buffer, buf_len * sizeof(ULONG));
    for(uint8_t i = 0; i < sizeof(lengths) / sizeof(SIZE_T); i++) {
        const char* result_text = passed_text;
        RtlFillMemoryUlong(buffer, lengths[i] * sizeof(ULONG), patterns[i]);
        for(uint8_t y = 0; y < buf_len; y++) {
            ULONG expected_pattern = (y < lengths[i]) ? patterns[i] : 0x0;
            if(buffer[y] != expected_pattern) {
                print("  ERROR: For index = %u, got = 0x%x, expected = 0x%x", y, buffer[y], expected_pattern);
                individual_test_passes = 0;
            }
        }
        if(!individual_test_passes) {
            tests_passed = 0;
            result_text = failed_text;
        }
        print("  Test %s for length = %u, pattern = 0x%x", result_text, lengths[i], patterns[i]);
        individual_test_passes = 1;
    }

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlFreeAnsiString(){
    const char* func_num = "0x011E";
    const char* func_name = "RtlFreeAnsiString";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const BOOL alloc_buffer = 1;
    ANSI_STRING ansi_string;
    UNICODE_STRING unicode_string;
    WCHAR unicode_text[] = L"Xbox";

    RtlInitUnicodeString(&unicode_string, unicode_text);
    RtlUnicodeStringToAnsiString(&ansi_string, &unicode_string, alloc_buffer);
    RtlFreeAnsiString(&ansi_string);
    tests_passed &= assert_ansi_string(
        &ansi_string,
        0,
        0,
        NULL,
        "Free ansi string"
    );

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlFreeUnicodeString(){
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

void stub_RtlGetCallersAddress(PVOID* CallerAddress_test, PVOID* CallersCaller_test, PULONG CallerAddress_expected){
    *CallerAddress_expected = (ULONG)__builtin_return_address(0);
    RtlGetCallersAddress(CallerAddress_test, CallersCaller_test);
}

void test_RtlGetCallersAddress(){
    const char* func_num = "0x0120";
    const char* func_name = "RtlGetCallersAddress";
    BOOL test_passed = 1;

    print_test_header(func_num, func_name);

    ULONG CallerAddress_test, CallersCaller_test, CallerAddress_expected;
    ULONG CallersCaller_expected = (ULONG)__builtin_return_address(0);

    stub_RtlGetCallersAddress((PVOID*)&CallerAddress_test, (PVOID*)&CallersCaller_test, &CallerAddress_expected);

    GEN_CHECK(CallerAddress_test, CallerAddress_expected, "CallerAddress");

    GEN_CHECK(CallersCaller_test, CallersCaller_expected, "CallersCaller");

    print_test_footer(func_num, func_name, test_passed);
}

void test_RtlInitAnsiString(){
    const char* func_num = "0x0121";
    const char* func_name = "RtlInitAnsiString";
    BOOL tests_passed = 1;
    ANSI_STRING ansi_string;
    CHAR string[] = "Xbox";

    print_test_header(func_num, func_name);

    ansi_string.Length = 100;
    ansi_string.MaximumLength = 200;
    RtlInitAnsiString(&ansi_string, NULL);
    tests_passed &= assert_ansi_string(
        &ansi_string,
        0,
        0,
        NULL,
        "Src str = NULL, Length = 0"
    );

    RtlInitAnsiString(&ansi_string, string);
    tests_passed &= assert_ansi_string(
        &ansi_string,
        strlen(string),
        strlen(string) + 1,
        string,
        "Use char array to create ansi string"
    );

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlInitUnicodeString(){
    const char* func_num = "0x0122";
    const char* func_name = "RtlInitUnicodeString";
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

void test_RtlInitializeCriticalSection(){
    const char* func_num = "0x0123";
    const char* func_name = "RtlInitializeCriticalSection";
    RTL_CRITICAL_SECTION crit_section;
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    RtlInitializeCriticalSection(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        -1,
        0,
        NULL,
        "Init critical section"
    );

    memset(&crit_section, 0x11, sizeof(crit_section));
    RtlInitializeCriticalSection(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        -1,
        0,
        NULL,
        "Re-Init critical section after setting garbage data"
    );

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlIntegerToChar(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlIntegerToUnicodeString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlLeaveCriticalSection(){
    const char* func_num = "0x0126";
    const char* func_name = "RtlLeaveCriticalSection";
    RTL_CRITICAL_SECTION crit_section;
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    RtlInitializeCriticalSection(&crit_section);
    RtlEnterCriticalSection(&crit_section);
    RtlEnterCriticalSection(&crit_section);
    RtlLeaveCriticalSection(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        0,
        1,
        (HANDLE)KeGetCurrentThread(),
        "Leave critical section once"
    );

    RtlLeaveCriticalSection(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        -1,
        0,
        NULL,
        "Leave critical section again"
    );

    RtlEnterCriticalSection(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        0,
        1,
        (HANDLE)KeGetCurrentThread(),
        "Re-Enter Critical Section"
    );

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlLeaveCriticalSectionAndRegion(){
    const char* func_num = "0x0127";
    const char* func_name = "RtlLeaveCriticalSectionAndRegion";
    RTL_CRITICAL_SECTION crit_section;
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    PKTHREAD thread = KeGetCurrentThread();
    const ULONG orig_APC_disable = thread->KernelApcDisable;

    RtlInitializeCriticalSection(&crit_section);
    RtlEnterCriticalSectionAndRegion(&crit_section);
    RtlEnterCriticalSectionAndRegion(&crit_section);

    RtlLeaveCriticalSectionAndRegion(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        0,
        1,
        (HANDLE)thread,
        "Leave critical section and region after entering twice"
    );
    tests_passed &= assert_critical_region(thread, orig_APC_disable - 2, "KernelApcDisable should be unchanged until critical section is freed");

    RtlLeaveCriticalSectionAndRegion(&crit_section);
    tests_passed &= assert_critical_section_equals(
        &crit_section,
        -1,
        0,
        NULL,
        "Leave critical section and region a second time"
    );
    tests_passed &= assert_critical_region(thread, orig_APC_disable - 1, "KernelApcDisable should now decrement as the critial section is freed");

    // Cleanup machine state
    KeLeaveCriticalRegion();

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlLowerChar(){
    const char* func_num = "0x0128";
    const char* func_name = "RtlLowerChar";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    // These results are taken from running inputs 0 through 255 on a NTSC Xbox
    CHAR expected_outputs[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xD7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
    };
    CHAR result;

    for(uint16_t i = 0; i < 0x100; i++) {
        result = RtlLowerChar((CHAR)i);
        if(result != expected_outputs[i]) {
            tests_passed = 0;
            print("  Test FAILED. Input = '%c'(0x%x), result = '%c'(0x%x), expected ='%c'(0x%x)",
                 (CHAR)i, i, result, (uint8_t)result, expected_outputs[i], (uint8_t)expected_outputs[i]);
        }
    }
    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlMapGenericMask(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlMoveMemory(){
    const char* func_num = "0x012A";
    const char* func_name = "RtlMoveMemory";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const int size = 1024;
    char rnd_letter;
    /* CREATE AND FILL THE SOURCE BUFFER */
    CHAR* src_buffer = malloc(sizeof(CHAR) * size);
    if(src_buffer == NULL) {
        print("ERROR: Could not malloc src_buffer");
    }
    srand(seed);
    for(int k=0; k<size; k++){
        rnd_letter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[rand() % 26];
        src_buffer[k] = rnd_letter;
    }

    /* CREATE AND FILL THE DESTINATION BUFFER (using RtlMoveMemory)*/
    CHAR* dest_buffer = malloc(sizeof(CHAR) * size);
    if(dest_buffer == NULL) {
        print("ERROR: Could not malloc dest_buffer");
    }
    RtlMoveMemory(dest_buffer, (void*)src_buffer, size);

    for(int k=0; k<size; k++){
        if(src_buffer[k] != dest_buffer[k]){
            tests_passed = 0;
            break;
        }
    }

    free(src_buffer);
    free(dest_buffer);
    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlMultiByteToUnicodeN(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlMultiByteToUnicodeSize(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlNtStatusToDosError(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlRaiseException(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlRaiseStatus(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlTimeFieldsToTime(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlTimeToTimeFields(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlTryEnterCriticalSection(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUlongByteSwap(){
    const char* func_num = "0x0133";
    const char* func_name = "RtlUlongByteSwap";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const ULONG inputs[]           = {0xFF00FF00, 0x12345678, 0xD4C3B2A1};
    const ULONG expected_results[] = {0x00FF00FF, 0x78563412, 0xA1B2C3D4};
    ULONG result;

    for(uint8_t i = 0; i < sizeof(inputs) / sizeof(ULONG); i++) {
        const char* result_text = passed_text;
        result = RtlUlongByteSwap(inputs[i]);
        if(result != expected_results[i]) {
            tests_passed = 0;
            result_text = failed_text;
        }
        print("  Test %s: Expected = 0x%x for Input = 0x%x, Result = 0x%x",
              result_text, expected_results[i], inputs[i], result);
    }

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlUnicodeStringToAnsiString(){
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
        sizeof(unicode_text) / sizeof(WCHAR) - 1,
        sizeof(unicode_text) / sizeof(WCHAR),
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
        sizeof(unicode_text) / sizeof(WCHAR) - 1,
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

void test_RtlUnicodeStringToInteger(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUnicodeToMultiByteN(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUnicodeToMultiByteSize(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUnwind(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUpcaseUnicodeChar(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUpcaseUnicodeString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUpcaseUnicodeToMultiByteN(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUpperChar(){
    const char* func_num = "0x013C";
    const char* func_name = "RtlUpperChar";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    // These results are taken from running inputs 0 through 255 on a NTSC Xbox
    CHAR expected_outputs[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xF7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0x3F,
    };
    CHAR result;

    for(uint16_t i = 0; i < 0x100; i++) {
        result = RtlUpperChar((CHAR)i);
        if(result != expected_outputs[i]) {
            tests_passed = 0;
            print("  Test FAILED. Input = '%c'(0x%x), result = '%c'(0x%x), expected ='%c'(0x%x)",
                 (CHAR)i, i, result, (uint8_t)result, expected_outputs[i], (uint8_t)expected_outputs[i]);
        }
    }

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlUpperString(){
    const char* func_num = "0x013D";
    const char* func_name = "RtlUpperString";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    char rnd_letter;
    char rnd_letters[101];

    srand(seed);
    for(int k=0; k<100; k++){
        rnd_letter = "abcdefghijklmnopqrstuvwxyz"[rand() % 26];
        rnd_letters[k] = rnd_letter;
    }
    rnd_letters[100] = '\0';

    ANSI_STRING src_str;
    ANSI_STRING res_str;
    char res_buf[256];

    /* Initialize res_buf so RtlInitAnsiString works correctly */
    for (int i=0; i<255; i++)
        res_buf[i] = '0';
    res_buf[255] = 0;

    RtlInitAnsiString(&res_str, res_buf);

    /* Empty String Test */
    RtlInitAnsiString(&src_str, "");
    RtlUpperString(&res_str, &src_str);
    tests_passed &= strncmp(res_str.Buffer, "", res_str.Length) == 0 ? 1 : 0;
    if(!tests_passed)
        print("RtlUpperString Lowercase String Test Failed");

    /* Lowercase String Test */
    RtlInitAnsiString(&src_str, "xbox");
    RtlUpperString(&res_str, &src_str);
    tests_passed &= strncmp(res_str.Buffer, "XBOX", res_str.Length) == 0 ? 1 : 0;
    if(!tests_passed)
        print("RtlUpperString Lowercase String Test Failed");

    /* Lowercase Single Character Test */
    RtlInitAnsiString(&src_str, "x");
    RtlUpperString(&res_str, &src_str);
    tests_passed &= strncmp(res_str.Buffer, "X", res_str.Length) == 0 ? 1 : 0;
    if(!tests_passed)
        print("RtlUpperString Lowercase Single Character Test Failed");

    /* Uppercase Single Character Test */
    RtlInitAnsiString(&src_str, "X");
    RtlUpperString(&res_str, &src_str);
    tests_passed &= strncmp(res_str.Buffer, "X", res_str.Length) == 0 ? 1 : 0;
    if(!tests_passed)
        print("RtlUpperString Uppercase Single Character Test Failed"); 

    /* 100 Lowercase Characters Test */
    RtlInitAnsiString(&src_str, rnd_letters);
    RtlUpperString(&res_str, &src_str);
    for(int k=0; k<100; k++){
        if(res_str.Buffer[k] != toupper(rnd_letters[k]))
            tests_passed = 0;
    }
    if(!tests_passed)
        print("RtlUpperString 100 Lowercase Characters Test Failed");

    /* Uppercase String Test */
    RtlInitAnsiString(&src_str, "XBOX");
    RtlUpperString(&res_str, &src_str);
    tests_passed &= strncmp(res_str.Buffer, "XBOX", res_str.Length) == 0 ? 1 : 0;
    if(!tests_passed)
        print("RtlUpperString Uppercase String Test Failed");

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlUshortByteSwap(){
    const char* func_num = "0x013E";
    const char* func_name = "RtlUshortByteSwap";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const uint8_t num_tests = 4;
    const USHORT inputs[num_tests]           = {0xFF00, 0x00FF, 0x1234, 0xF0E0};
    const USHORT expected_results[num_tests] = {0x00FF, 0xFF00, 0x3412, 0xE0F0};
    USHORT result;

    for(uint8_t i = 0; i < num_tests; i++) {
        const char* result_text = passed_text;
        result = RtlUshortByteSwap(inputs[i]);
        if(result != expected_results[i]) {
            tests_passed = 0;
            result_text = failed_text;
        }
        print("  Test %s: Expected = 0x%x for Input = 0x%x, Result = 0x%x",
              result_text, expected_results[i], inputs[i], result);
    }

    print_test_footer(func_num, func_name, tests_passed);
}

// stack calls to see how RtlWalkFrameChain work internally.
static ULONG stub_RtlWalkFrameChain4of4(PVOID* Callers, ULONG Count) {
    return RtlWalkFrameChain(Callers, Count, 0/*Flags parameter is unused*/);
}
static ULONG stub_RtlWalkFrameChain3of4(PVOID* Callers, ULONG Count) {
    return stub_RtlWalkFrameChain4of4(Callers, Count);
}
static ULONG stub_RtlWalkFrameChain2of4(PVOID* Callers, ULONG Count) {
    return stub_RtlWalkFrameChain3of4(Callers, Count);
}
static ULONG stub_RtlWalkFrameChain1of4(PVOID* Callers, ULONG Count) {
    return stub_RtlWalkFrameChain2of4(Callers, Count);
}

void test_RtlWalkFrameChain(){
    const char* func_num = "0x013F";
    const char* func_name = "RtlWalkFrameChain";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    ULONG count = 20;
    ULONG callers[count];
    memset(callers, 0, sizeof(callers));

    ULONG total = stub_RtlWalkFrameChain1of4((PVOID*)callers, count);

    // Last time checked on hardware, there's total of 10 calls.
    GEN_CHECK(total, 10, "total");

    // We are checking for child calls in this function for verification.
    GEN_CHECK_RANGE(callers[0], (ULONG)(PVOID)stub_RtlWalkFrameChain4of4, 0x50, "callers[0]");
    GEN_CHECK_RANGE(callers[1], (ULONG)(PVOID)stub_RtlWalkFrameChain3of4, 0x30, "callers[1]");
    GEN_CHECK_RANGE(callers[2], (ULONG)(PVOID)stub_RtlWalkFrameChain2of4, 0x30, "callers[2]");
    GEN_CHECK_RANGE(callers[3], (ULONG)(PVOID)stub_RtlWalkFrameChain1of4, 0x30, "callers[3]");

    // We don't check test_RtlWalkFrameChain as it can change at any time.

    // Output any remaining parent calls occur from the stack.
    // We do not make any parent calls address verification as they can change at any time.
    for (ULONG i = 4; i < total; i++) {
        print("  INFO: callers[%u] = 0x%08x", i, callers[i]);
    }

    print_test_footer(func_num, func_name, test_passed);
}

void test_RtlZeroMemory(){
    const char* func_num = "0x0140";
    const char* func_name = "RtlZeroMemory";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const int size = 1024;
    CHAR* alloc_buffer = malloc(sizeof(CHAR) * size);
    if(alloc_buffer == NULL) {
        print("ERROR: Could not malloc alloc_buffer");
    }
    RtlZeroMemory(alloc_buffer, size);
    for(int k=0; k<size; k++){
        if(alloc_buffer[k] != (char)0){
            tests_passed = 0;
            break;
        }
    }
    free(alloc_buffer);
    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlRip(){
    /* FIXME: This is a stub! implement this function! */
}
