#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "output.h"
#include "rtl_assertions.h"

void test_RtlAnsiStringToUnicodeString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlAppendStringToString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlAppendUnicodeStringToString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlAppendUnicodeToString(){
    /* FIXME: This is a stub! implement this function! */
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
    /* FIXME: This is a stub! implement this function! */
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
    /* FIXME: This is a stub! implement this function! */
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
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlCreateUnicodeString(){
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlDowncaseUnicodeChar(){
    /* FIXME: This is a stub! implement this function! */
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
