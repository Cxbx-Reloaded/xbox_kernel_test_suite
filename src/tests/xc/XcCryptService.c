#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"

void test_XcCryptService(int func_num, const char* func_name)
{
	BOOL test_passed = 1;
	ULONG a;
	void* b;
	ULONG ret;
	UCHAR test_buffer[8] = { 0 };

	print_test_header(func_num, func_name);
	
	a = 0;
	b = NULL;
	ret = XcCryptService(a, b);
	if(ret == 0 && a == 0 && b == NULL) {
		test_passed &= 1;
	}
	else {
		test_passed &= 0;
	}
	
	a = 0xFFFFFFFF;
	b = NULL;
	ret = XcCryptService(a, b);
	if(ret == 0 && a == 0xFFFFFFFF && b == NULL) {
		test_passed &= 1;
	}
	else {
		test_passed &= 0;
	}
	
	a = 0xFFFFFFFF;
	b = test_buffer;
	ret = XcCryptService(a, b);
	if(ret == 0 && a == 0xFFFFFFFF && (memcmp(b, "\x00\x00\x00\x00\x00\x00\x00\x00", 8) == 0)) {
		test_passed &= 1;
	}
	else {
		test_passed &= 0;
	}
	
	a = 0;
	b = test_buffer;
	ret = XcCryptService(a, b);
	if(ret == 0 && a == 0 && (memcmp(b, "\x00\x00\x00\x00\x00\x00\x00\x00", 8) == 0)) {
		test_passed &= 1;
	}
	else {
		test_passed &= 0;
	}
	
	print_test_footer(func_num, func_name, test_passed);
}
