#ifndef OUTPUT_H
#define OUTPUT_H

#include <xboxkrnl/xboxkrnl.h>

void print(char* str, ...);
void print_test_header(const char*, const char*);
void print_test_footer(const char*, const char*, BOOL);

#endif
