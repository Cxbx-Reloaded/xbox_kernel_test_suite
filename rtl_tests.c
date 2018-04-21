#include <xboxkrnl/xboxkrnl.h>
#include <string.h>
#include <stdlib.h>

#include "output.h"
#include "common_assertions.h"
#include "rtl_assertions.h"

// TODO - Move into nxdk
#define STATUS_INVALID_PARAMETER_2  0xC00000F0
#define STATUS_BUFFER_OVERFLOW      0x80000005
#define STATUS_BUFFER_TOO_SMALL     0xC0000023

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

void test_RtlAppendStringToString(){
    const char* func_num = "0x0105";
    const char* func_name = "RtlAppendStringToString";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    const uint8_t buf_size = 8;
    const CHAR src_text[] = "Xbox";
    ANSI_STRING src_str, dest_str;
    CHAR buffer[buf_size];

    RtlInitAnsiString(&src_str, src_text);
    dest_str.Length = 0;
    dest_str.MaximumLength = buf_size;
    dest_str.Buffer = buffer;

    NTSTATUS ret = RtlAppendStringToString(&dest_str, &src_str);
    tests_passed &= assert_NTSTATUS(ret, STATUS_SUCCESS, func_name);
    tests_passed &= assert_ansi_string(
        &dest_str,
        strlen(src_text),
        buf_size,
        buffer,
        "Append src str to empty dest str"
    );

    ret = RtlAppendStringToString(&dest_str, &src_str);
    tests_passed &= assert_NTSTATUS(ret, STATUS_SUCCESS, func_name);
    tests_passed &= assert_ansi_string(
        &dest_str,
        strlen(src_text) * 2,
        buf_size,
        buffer,
        "Append src str to dest str again"
    );

    ret = RtlAppendStringToString(&dest_str, &src_str);
    tests_passed &= assert_NTSTATUS(ret, STATUS_BUFFER_TOO_SMALL, func_name);

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlAppendUnicodeStringToString(){
    const char* func_num = "0x0106";
    const char* func_name = "RtlAppendUnicodeStringToString";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    // wcslen does not work because nxdk fails to find wchar.h
    const WCHAR src_text[] = L"Xbox";
    const uint8_t num_chars_in_src = 4;
    WCHAR buffer[num_chars_in_src * 2];
    const uint8_t num_buf_bytes = sizeof(buffer);
    UNICODE_STRING src_str, dest_str;

    RtlInitUnicodeString(&src_str, src_text);
    dest_str.Length = 0;
    dest_str.MaximumLength = num_buf_bytes;
    dest_str.Buffer = buffer;

    NTSTATUS ret = RtlAppendUnicodeStringToString(&dest_str, &src_str);
    tests_passed &= assert_NTSTATUS(ret, STATUS_SUCCESS, func_name);
    tests_passed &= assert_unicode_string(
        &dest_str,
        num_chars_in_src * sizeof(WCHAR),
        num_buf_bytes,
        buffer,
        "Append src str to empty dest str"
    );

    ret = RtlAppendUnicodeStringToString(&dest_str, &src_str);
    tests_passed &= assert_NTSTATUS(ret, STATUS_SUCCESS, func_name);
    tests_passed &= assert_unicode_string(
        &dest_str,
        num_chars_in_src * 2 * sizeof(WCHAR),
        num_buf_bytes,
        buffer,
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

    // wcslen does not work because nxdk fails to find wchar.h
    const WCHAR src_text[] = L"Xbox";
    const uint8_t num_chars_in_src = 4;
    WCHAR buffer[num_chars_in_src * 2];
    const uint8_t num_buf_bytes = sizeof(buffer);
    UNICODE_STRING dest_str;

    dest_str.Length = 0;
    dest_str.MaximumLength = num_buf_bytes;
    dest_str.Buffer = buffer;

    NTSTATUS ret = RtlAppendUnicodeToString(&dest_str, src_text);
    tests_passed &= assert_NTSTATUS(ret, STATUS_SUCCESS, func_name);
    tests_passed &= assert_unicode_string(
        &dest_str,
        num_chars_in_src * sizeof(WCHAR),
        num_buf_bytes,
        buffer,
        "Append src str to empty dest str"
    );

    ret = RtlAppendUnicodeToString(&dest_str, src_text);
    tests_passed &= assert_NTSTATUS(ret, STATUS_SUCCESS, func_name);
    tests_passed &= assert_unicode_string(
        &dest_str,
        num_chars_in_src * 2 * sizeof(WCHAR),
        num_buf_bytes,
        buffer,
        "Append src str to dest str again"
    );

    ret = RtlAppendUnicodeToString(&dest_str, src_text);
    tests_passed &= assert_NTSTATUS(ret, STATUS_BUFFER_TOO_SMALL, func_name);

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlAssert(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlCaptureContext(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlCaptureStackBackTrace(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlCharToInteger(){
    const char* func_num = "0x010B";
    const char* func_name = "RtlCharToInteger";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    unsigned long val = 0;
    NTSTATUS res;

    res = RtlCharToInteger("-1", 0, &val);
    if(res == STATUS_SUCCESS && val == -1)
        tests_passed &= 1;
    else
        tests_passed = 0;

    res = RtlCharToInteger("0", 0, &val);
    if(res == STATUS_SUCCESS && val == 0)
        tests_passed &= 1;
    else
        tests_passed = 0;

    res = RtlCharToInteger("1", 0, &val);
    if(res == STATUS_SUCCESS && val == 1)
        tests_passed &= 1;
    else
        tests_passed = 0;

    res = RtlCharToInteger("0x12345", 0, &val);
    if(res == STATUS_SUCCESS && val == 0x12345)
        tests_passed &= 1;
    else
        tests_passed = 0;

    res = RtlCharToInteger("1011101100", 2, &val);
    if(res == STATUS_SUCCESS && val == 748)
        tests_passed &= 1;
    else
        tests_passed = 0;

    res = RtlCharToInteger("-1011101100", 2, &val);
    if(res == STATUS_SUCCESS && val == -748)
        tests_passed &= 1;
    else
        tests_passed = 0;

    res = RtlCharToInteger("1011101100", 8, &val);
    if(res == STATUS_SUCCESS && val == 136610368)
        tests_passed &= 1;
    else
        tests_passed = 0;

    res = RtlCharToInteger("-1011101100", 8, &val);
    if(res == STATUS_SUCCESS && val == -136610368)
        tests_passed &= 1;
    else
        tests_passed = 0;

    res = RtlCharToInteger("1011101100", 10, &val);
    if(res == STATUS_SUCCESS && val == 1011101100)
        tests_passed &= 1;
    else
        tests_passed = 0;

    res = RtlCharToInteger("-1011101100", 10, &val);
    if(res == STATUS_SUCCESS && val == -1011101100)
        tests_passed &= 1;
    else
        tests_passed = 0;

    res = RtlCharToInteger("1011101100", 16, &val);
    if(res == STATUS_SUCCESS && val == 286265600)
        tests_passed &= 1;
    else
        tests_passed = 0;

    res = RtlCharToInteger("0", 20, &val);
    if(res == STATUS_INVALID_PARAMETER)
        tests_passed &= 1;
    else
        tests_passed = 0;


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
        result = RtlDowncaseUnicodeChar(input[i]);
        if(result == expected_output[i]) {
            // For some reason print does not work with %S to display input chars
            print("  Test PASSED for input #%u", i);
        }
        else {
            tests_passed = 0;
            print("  Test FAILED for input #%u", i);
        }
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
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlEqualString(){ // FIXME this test failed on real hardware!!!
    /*const char* func_num = "0x0117";
    const char* func_name = "RtlEqualString";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    ANSI_STRING str1;
    ANSI_STRING str2;
    ANSI_STRING str3;

    RtlInitAnsiString(&str1, "xbox is cool");
    RtlInitAnsiString(&str2, "XBOX IS COOL");
    RtlInitAnsiString(&str3, "is xbox cool?");

    tests_passed &= !RtlEqualString(&str1, &str2, 0);
    print("1st result=%d", RtlEqualString(&str1, &str2, 0));
    tests_passed &= RtlEqualString(&str1, &str2, 1);
    print("2nd result=%d", RtlEqualString(&str1, &str2, 1));
    tests_passed &= !RtlEqualString(&str1, &str3, 1);
    print("3rd result=%d", RtlEqualString(&str1, &str3, 1));
    tests_passed &= !RtlEqualString(&str1, &str3, 0);
    print("4th result=%d", RtlEqualString(&str1, &str3, 0));

    print_test_footer(func_num, func_name, tests_passed);*/
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
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlExtendedLargeIntegerDivide(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlExtendedMagicDivide(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlFillMemory(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlFillMemoryUlong(){
    /* FIXME: This is a stub! implement this function! */
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

void test_RtlGetCallersAddress(){
    /* FIXME: This is a stub! implement this function! */
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
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlLowerChar(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlMapGenericMask(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlMoveMemory(){
    /* FIXME: This is a stub! implement this function! */
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
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUnicodeStringToAnsiString(){
    /* FIXME: This is a stub! implement this function! */
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
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUpperString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUshortByteSwap(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlWalkFrameChain(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlZeroMemory(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlRip(){
    /* FIXME: This is a stub! implement this function! */
}
