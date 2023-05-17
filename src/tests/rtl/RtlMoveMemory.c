#include <xboxkrnl/xboxkrnl.h>
#include <stdlib.h>

#include "global.h"
#include "util/output.h"

void test_RtlMoveMemory()
{
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
