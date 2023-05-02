#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"

void test_XcModExp(){
	const char* func_num = "0x0159";
    const char* func_name = "XcModExp";
    BOOL test_passed = 1;
	ULONG ret = 0;
	ULONG output = 0;
	ULONG a      = 0x10;
	ULONG b      = 0x20;
	ULONG c      = 0x30;

    print_test_header(func_num, func_name);

	ret = XcModExp(&output, &a, &b, &c, 4/sizeof(ULONG));
	if(ret == 1 && output == 0x10) {
        test_passed &= 1;
	}
    else {
        test_passed &= 0;
	}

	// Passing a size of zero freezes (crash?) my xbox so I won't test it

	print_test_footer(func_num, func_name, test_passed);
}
