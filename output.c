#include <pbkit/pbkit.h>
#include <stdarg.h>
#include <stdlib.h>

void print(char* str, ...){
     va_list args;
     char buffer[500];
     va_start (args, str);
     vsnprintf (buffer, 499, str, args);
	 va_end(args);
     DbgPrint(buffer);
}
