#include <xboxkrnl/xboxkrnl.h>
#include <stdint.h>

#include "global.h" // for (passed|failed)_test vars
#include "util/output.h"
#include "assertions/rtl.h"

void test_RtlCompareMemory()
{
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

void test_RtlCompareMemoryUlong()
{
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

void test_RtlCompareString()
{
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

void test_RtlCompareUnicodeString()
{
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

void test_RtlEqualString()
{
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

void test_RtlEqualUnicodeString()
{
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
