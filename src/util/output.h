#pragma once

#include <xboxkrnl/xboxdef.h>

void print(const char* str, ...);
void print_test_header(int, const char*);
void print_test_footer(int, const char*, BOOL);

#define TEST_FUNC(name) void test_ ## name(int api_num, const char* api_name)
#define TEST_GET_VAR test_passed
#define TEST_GET_API_N api_num
#define TEST_GET_API_NAME api_name
#define TEST_BEGIN() print_test_header(api_num, api_name); \
    BOOL TEST_GET_VAR = 1
#define TEST_END() print_test_footer(api_num, api_name, test_passed)
#define TEST_UNIMPLEMENTED()
#define TEST_FAILED() TEST_GET_VAR = 0
#define TEST_IS_FAILED (!TEST_GET_VAR)
#define TEST_IS_SUCCESS (TEST_GET_VAR)

// Real hardware can only display one screen of text at a time. Create an output
// logfile to contain information for all tests.
BOOL open_output_file(const char*);
int write_to_output_file(void*, DWORD);
void close_output_file();
