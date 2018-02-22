#include <pbkit/pbkit.h>
#include <xboxrt/debug.h>
#include <stdarg.h>
#include <stdlib.h>

void print(char* str, ...){
    va_list args;
    char buffer[500];
    va_start (args, str);
    vsnprintf (buffer, 499, str, args);
    va_end(args);
    /**** PRINT ON TV (REAL HW) ****/
    debugPrint(buffer);
    debugPrint("\n");
    /*******************************/

    /*** PRINT ON CONSOLE (CXBX) ***/
    DbgPrint("%s\n", buffer);
    /*******************************/
}

void print_test_header(const char* func_num, const char* func_name) {
    print("%s - %s: Tests Starting", func_num, func_name);
}

void print_test_footer(
    const char* func_num, const char* func_name, BOOL tests_passed
) {
    if(tests_passed) {
        print("%s - %s: All tests PASSED", func_num, func_name);
    }
    else {
        print("%s - %s: One or more tests FAILED", func_num, func_name);
    }
}
