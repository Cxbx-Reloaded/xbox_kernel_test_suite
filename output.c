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
