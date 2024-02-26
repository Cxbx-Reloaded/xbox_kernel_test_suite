#include <xboxkrnl/xboxkrnl.h>
#include <stdint.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(RtlCompareMemory)
{
    TEST_BEGIN();

    uint32_t num1 = 0;
    uint32_t num2 = 0;
    VOID* num1_ptr = (VOID*)&num1;
    VOID* num2_ptr = (VOID*)&num2;

    SIZE_T bytes_matching = RtlCompareMemory(num1_ptr, num2_ptr, 0);
    GEN_CHECK(bytes_matching, 0, "Check 0 bytes of memory");

    bytes_matching = RtlCompareMemory(num1_ptr, num2_ptr, sizeof(num1));
    GEN_CHECK(bytes_matching, sizeof(num1), "Both nums 0, should match");

    num1 |= 0x80000000;
    bytes_matching = RtlCompareMemory(num1_ptr, num2_ptr, sizeof(num1));
    GEN_CHECK(bytes_matching, sizeof(num1) - 1, "Only 3 bytes should match");

    num2 |= 0x4000;
    bytes_matching = RtlCompareMemory(num1_ptr, num2_ptr, sizeof(num1));
    GEN_CHECK(bytes_matching, sizeof(num1) - 3, "Only 1 byte should match");

    num2 |= 0x80000000;
    num1 |= 0x4000;
    bytes_matching = RtlCompareMemory(num1_ptr, num2_ptr, sizeof(num1));
    GEN_CHECK(bytes_matching, sizeof(num1), "All bytes should match again");

    TEST_END();
}

TEST_FUNC(RtlCompareMemoryUlong)
{
    TEST_BEGIN();

    ULONG mem[4];
    ULONG res;
    mem[0]= 0x0123;
    mem[1]= 0x4567;
    mem[2]= 0x89ab;
    mem[3]= 0xcdef;

    SIZE_T result;
    result = RtlCompareMemoryUlong(mem, 0, 0x0123);
    GEN_CHECK(result, 0, "result");
    result = RtlCompareMemoryUlong(mem, 3, 0x0123);
    GEN_CHECK(result, 0, "result");
    result = RtlCompareMemoryUlong(mem, 4, 0x0123);
    GEN_CHECK(result, 4, "result");
    result = RtlCompareMemoryUlong(mem, 5, 0x0123);
    GEN_CHECK(result, 4, "result");
    result = RtlCompareMemoryUlong(mem, 7, 0x0123);
    GEN_CHECK(result, 4, "result");
    result = RtlCompareMemoryUlong(mem, 8, 0x0123);
    GEN_CHECK(result, 4, "result");
    result = RtlCompareMemoryUlong(mem, 9, 0x0123);
    GEN_CHECK(result, 4, "result");
    result = RtlCompareMemoryUlong(mem, 4, 0x0127);
    GEN_CHECK(result, 0, "result");
    result = RtlCompareMemoryUlong(mem, 4, 0x7123);
    GEN_CHECK(result, 0, "result");
    result = RtlCompareMemoryUlong(mem, 16, 0x4567);
    GEN_CHECK(result, 0, "result");

    TEST_END();
}

TEST_FUNC(RtlCompareString)
{
    TEST_BEGIN();

    ANSI_STRING lower_case_string;
    ANSI_STRING upper_case_string;
    ANSI_STRING long_string;

    RtlInitAnsiString(&lower_case_string, "test_string");
    RtlInitAnsiString(&upper_case_string, "TEST_STRING");
    RtlInitAnsiString(&long_string, "this_is_a_long_string");

    LONG result;

    result = RtlCompareString(&lower_case_string, &upper_case_string, 1);
    GEN_CHECK(result, 0, "result");
    result = RtlCompareString(&lower_case_string, &upper_case_string, 0);
    GEN_CHECK(result, 32, "result");
    result = RtlCompareString(&lower_case_string, &lower_case_string, 1);
    GEN_CHECK(result, 0, "result");
    result = RtlCompareString(&lower_case_string, &lower_case_string, 0);
    GEN_CHECK(result, 0, "result");
    result = RtlCompareString(&lower_case_string, &long_string, 1);
    GEN_CHECK(result, -3, "result");
    result = RtlCompareString(&long_string, &lower_case_string, 1);
    GEN_CHECK(result, 3, "result");

    TEST_END();
}

TEST_FUNC(RtlCompareUnicodeString)
{
    TEST_BEGIN();

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
    GEN_CHECK(result, 0, "result");

    result = RtlCompareUnicodeString(&base_str, &lowercase_base_str, case_sensitive);
    GEN_CHECK(result, -32, "result");

    result = RtlCompareUnicodeString(&base_str, &lowercase_base_str, case_insensitive);
    GEN_CHECK(result, 0, "result");

    result = RtlCompareUnicodeString(&base_str, &shorter_str, case_sensitive);
    GEN_CHECK(result, 4, "result");

    result = RtlCompareUnicodeString(&base_str, &shorter_str, case_insensitive);
    GEN_CHECK(result, 4, "result");

    result = RtlCompareUnicodeString(&base_str, &longer_str, case_sensitive);
    GEN_CHECK(result, -18, "result");

    result = RtlCompareUnicodeString(&base_str, &longer_str, case_insensitive);
    GEN_CHECK(result, -18, "result");

    TEST_END();
}
static const char* str1_str = "str1";
static const char* str2_str = "str2";
static const char* str3_str = "str3";
static const char* case_insensitive_str = "insensitive";
static const char* case_sensitive_str = "sensitive";

TEST_FUNC(RtlEqualString)
{
    TEST_BEGIN();

    ANSI_STRING str1, str2, str3;
    RtlInitAnsiString(&str1, "xbox is cool");
    RtlInitAnsiString(&str2, "XBOX IS COOL");
    RtlInitAnsiString(&str3, "is xbox cool?");

    typedef struct _eq_str_test {
        const char* var_name;
        ANSI_STRING* str_compare;
        BOOLEAN case_insensitive;
        BOOLEAN expected_result;
        BOOLEAN return_result;
    } eq_str_test;

    eq_str_test str_tests[] = {
        { .var_name = str1_str, .str_compare = &str1, .case_insensitive = 0, .expected_result = 1 },
        { .var_name = str1_str, .str_compare = &str1, .case_insensitive = 1, .expected_result = 1 },
        { .var_name = str2_str, .str_compare = &str2, .case_insensitive = 0, .expected_result = 0 },
        { .var_name = str2_str, .str_compare = &str2, .case_insensitive = 1, .expected_result = 1 },
        { .var_name = str3_str, .str_compare = &str3, .case_insensitive = 0, .expected_result = 0 },
        { .var_name = str3_str, .str_compare = &str3, .case_insensitive = 1, .expected_result = 0 }
    };
    unsigned str_tests_size = ARRAY_SIZE(str_tests);

    for (unsigned i = 0; i < str_tests_size; i++) {
        str_tests[i].return_result = RtlEqualString(&str1, str_tests[i].str_compare, str_tests[i].case_insensitive);
    }
    GEN_CHECK_ARRAY_MEMBER(str_tests, return_result, expected_result, str_tests_size, "str_tests");

    TEST_END();
}

TEST_FUNC(RtlEqualUnicodeString)
{
    TEST_BEGIN();

    UNICODE_STRING str1;
    UNICODE_STRING str2;
    UNICODE_STRING str3;

    RtlInitUnicodeString(&str1, L"xbox is cool");
    RtlInitUnicodeString(&str2, L"XBOX IS COOL");
    RtlInitUnicodeString(&str3, L"is xbox cool?");

    typedef struct _eq_str_test {
        const char* var_name;
        UNICODE_STRING* str_compare;
        BOOLEAN case_insensitive;
        BOOLEAN expected_result;
        BOOLEAN return_result;
    } eq_str_test;

    eq_str_test str_tests[] = {
        { .var_name = str1_str, .str_compare = &str1, .case_insensitive = 0, .expected_result = 1 },
        { .var_name = str1_str, .str_compare = &str1, .case_insensitive = 1, .expected_result = 1 },
        { .var_name = str2_str, .str_compare = &str2, .case_insensitive = 0, .expected_result = 0 },
        { .var_name = str2_str, .str_compare = &str2, .case_insensitive = 1, .expected_result = 1 },
        { .var_name = str3_str, .str_compare = &str3, .case_insensitive = 0, .expected_result = 0 },
        { .var_name = str3_str, .str_compare = &str3, .case_insensitive = 1, .expected_result = 0 }
    };
    unsigned str_tests_size = ARRAY_SIZE(str_tests);

    for (unsigned i = 0; i < str_tests_size; i++) {
        str_tests[i].return_result = RtlEqualUnicodeString(&str1, str_tests[i].str_compare, str_tests[i].case_insensitive);
    }
    GEN_CHECK_ARRAY_MEMBER(str_tests, return_result, expected_result, str_tests_size, "str_tests");

    TEST_END();
}
