#pragma once

#include <xboxkrnl/xboxdef.h>

void print(char* str, ...);
void print_test_header(int, const char*);
void print_test_footer(int, const char*, BOOL);

// Real hardware can only display one screen of text at a time. Create an output
// logfile to contain information for all tests.
BOOL open_output_file(char*);
int write_to_output_file(void*, DWORD);
void close_output_file();
