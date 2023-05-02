#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"

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
