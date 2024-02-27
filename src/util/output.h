#pragma once

#include <xboxkrnl/xboxdef.h>

void print(const char* str, ...);
void print_test_header(int, const char*);
void print_test_footer(int, const char*, BOOL);

#define TEST_FUNC(name) void test_ ## name(int api_num, const char* api_name)
#define TEST_BEGIN() print_test_header(api_num, api_name); \
    BOOL test_passed = 1
#define TEST_END() print_test_footer(api_num, api_name, test_passed)
#define TEST_UNIMPLEMENTED()
#define TEST_FAILED() test_passed = 0
#define TEST_IS_FAILED (!test_passed)
#define TEST_IS_SUCCESS (test_passed)

// Real hardware can only display one screen of text at a time. Create an output
// logfile to contain information for all tests.
BOOL open_output_file(const char*);
int write_to_output_file(void*, DWORD);
void close_output_file();
