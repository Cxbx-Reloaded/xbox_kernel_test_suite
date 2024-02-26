#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"

TEST_FUNC(RtlCopyString)
{
    TEST_BEGIN();

    char src_char_arr[] = "xbox is cool";
    char dst_char_arr[] = "            ";
    STRING src_str;
    STRING dst_str;

    RtlInitAnsiString(&src_str, src_char_arr);
    RtlInitAnsiString(&dst_str, dst_char_arr);
    RtlCopyString(&dst_str, &src_str);

    if(strncmp(src_str.Buffer, dst_char_arr, src_str.Length) != 0) {
        TEST_FAILED();
    }

    TEST_END();
}
