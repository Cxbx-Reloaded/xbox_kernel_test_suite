#include <xboxkrnl/xboxkrnl.h>
#include <stdlib.h>

#include "util/output.h"

void test_RtlZeroMemory()
{
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
