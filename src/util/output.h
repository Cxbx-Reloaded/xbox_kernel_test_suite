#pragma once

#include <xboxkrnl/xboxdef.h>

void print(char* str, ...);
void print_test_header(const char*, const char*);
void print_test_footer(const char*, const char*, BOOL);

// Real hardware can only display one screen of text at a time. Create an output
// logfile to contain information for all tests.
void open_output_file(char*);
int write_to_output_file(void*, DWORD);
BOOL close_output_file();
