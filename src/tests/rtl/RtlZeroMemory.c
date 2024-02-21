#include <xboxkrnl/xboxkrnl.h>
#include <stdlib.h>

#include "util/output.h"

TEST_FUNC(RtlZeroMemory)
{
    TEST_BEGIN();

    const int size = 1024;
    CHAR* alloc_buffer = malloc(sizeof(CHAR) * size);
    if(alloc_buffer == NULL) {
        print("ERROR: Could not malloc alloc_buffer");
    }
    RtlZeroMemory(alloc_buffer, size);
    for(int k=0; k<size; k++){
        if(alloc_buffer[k] != (char)0){
            TEST_FAILED();
            break;
        }
    }
    free(alloc_buffer);

    TEST_END();
}
