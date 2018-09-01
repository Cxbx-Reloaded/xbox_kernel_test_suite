#include <xboxkrnl/xboxkrnl.h>
#include <hal/xbox.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "output.h"
#include "common_assertions.h"
#include "rtl_assertions.h"
#include "ke_assertions.h"

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
        RtlFillMemory(buffer, lengths[i], fills[i]);
        for(uint8_t y = 0; y < buf_len; y++) {
            BYTE expected_fill = (y < lengths[i]) ? fills[i] : 0x00;
            if(buffer[y] != expected_fill) {
                print("  ERROR: For index = %u, got = 0x%x, expected = 0x%x", y, buffer[y], expected_fill);
                individual_test_passes = 0;
            }
        }
        if(individual_test_passes) {
            print("  Test PASSED for length = %u, fill = 0x%x", lengths[i], fills[i]);
        }
        else {
            print("  Test FAILED for length = %u, fill = 0x%x", lengths[i], fills[i]);
            tests_passed = 0;
        }
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
        RtlFillMemoryUlong(buffer, lengths[i] * sizeof(ULONG), patterns[i]);
        for(uint8_t y = 0; y < buf_len; y++) {
            ULONG expected_pattern = (y < lengths[i]) ? patterns[i] : 0x0;
            if(buffer[y] != expected_pattern) {
                print("  ERROR: For index = %u, got = 0x%x, expected = 0x%x", y, buffer[y], expected_pattern);
                individual_test_passes = 0;
            }
        }
        if(individual_test_passes) {
            print("  Test PASSED for length = %u, pattern = 0x%x", lengths[i], patterns[i]);
        }
        else {
            print("  Test FAILED for length = %u, pattern = 0x%x", lengths[i], patterns[i]);
            tests_passed = 0;
        }
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
    for(int k=0; k<size; k++){ // we use XGetTickCount as a rand() replacement
        rnd_letter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[(int)XGetTickCount() % 26];
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

// FIXME - This test hangs on real hardware but passes on Cxbx-R
void test_RtlUpperString(){
//    const char* func_num = "0x013D";
//    const char* func_name = "RtlUpperString";
//    BOOL tests_passed = 1;
//    print_test_header(func_num, func_name);
//
//    char rnd_letter;
//    char rnd_letters[101];
//
//    for(int k=0; k<100; k++){ // we use XGetTickCount as a rand() replacement
//        rnd_letter = "abcdefghijklmnopqrstuvwxyz"[(int)XGetTickCount() % 26];
//        rnd_letters[k] = rnd_letter;
//    }
//    rnd_letters[100] = '\0';
//
//    ANSI_STRING src_str;
//    ANSI_STRING res_str;
//
//    /* Empty String Test */
//    RtlInitAnsiString(&src_str, "");
//    RtlInitAnsiString(&res_str, "");
//    RtlUpperString(&res_str, &src_str);
//    tests_passed &= strcmp(res_str.Buffer, "") == 0 ? 1 : 0;
//    if(!tests_passed)
//        print("RtlUpperString Lowercase String Test Failed");
//
//    /* Lowercase String Test */
//    RtlInitAnsiString(&src_str, "xbox");
//    RtlInitAnsiString(&res_str, "xbox");
//    RtlUpperString(&res_str, &src_str);
//    tests_passed &= strcmp(res_str.Buffer, "XBOX") == 0 ? 1 : 0;
//    if(!tests_passed)
//        print("RtlUpperString Lowercase String Test Failed");
//
//    /* Lowercase Single Character Test */
//    RtlInitAnsiString(&src_str, "x");
//    RtlInitAnsiString(&res_str, "x");
//    RtlUpperString(&res_str, &src_str);
//    tests_passed &= strcmp(res_str.Buffer, "X") == 0 ? 1 : 0;
//    if(!tests_passed)
//        print("RtlUpperString Lowercase Single Character Test Failed");
//
//    /* Uppercase Single Character Test */
//    RtlInitAnsiString(&src_str, "X");
//    RtlInitAnsiString(&res_str, "X");
//    RtlUpperString(&res_str, &src_str);
//    tests_passed &= strcmp(res_str.Buffer, "X") == 0 ? 1 : 0;
//    if(!tests_passed)
//        print("RtlUpperString Uppercase Single Character Test Failed"); 
//
//    /* 100 Lowercase Characters Test */
//    RtlInitAnsiString(&src_str, rnd_letters);
//    RtlInitAnsiString(&res_str, rnd_letters);
//    RtlUpperString(&res_str, &src_str);
//    for(int k=0; k<100; k++){
//        if(res_str.Buffer[k] != toupper(rnd_letters[k]))
//            tests_passed = 0;
//    }
//    if(!tests_passed)
//        print("RtlUpperString 100 Lowercase Characters Test Failed");
//
//    /* Uppercase String Test */
//    RtlInitAnsiString(&src_str, "XBOX");
//    RtlInitAnsiString(&res_str, "XBOX");
//    RtlUpperString(&res_str, &src_str);
//    tests_passed &= strcmp(res_str.Buffer, "XBOX") == 0 ? 1 : 0;
//    if(!tests_passed)
//        print("RtlUpperString Uppercase String Test Failed");
//
//    print_test_footer(func_num, func_name, tests_passed);
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
        result = RtlUshortByteSwap(inputs[i]);
        if(result == expected_results[i]) {
            print("  Test PASSED: Expected = 0x%x for Input = 0x%x, Result = 0x%x",
                  expected_results[i], inputs[i], result);
        }
        else {
            tests_passed = 0;
            print("  Test FAILED: Expected = 0x%x for Input = 0x%x, Result = 0x%x",
                  expected_results[i], inputs[i], result);
        }
    }

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlWalkFrameChain(){
    /* FIXME: This is a stub! implement this function! */
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
