#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"

TEST_FUNC(XcModExp)
{
    TEST_BEGIN();

	ULONG ret = 0;
	ULONG output = 0;
	ULONG a      = 0x10;
	ULONG b      = 0x20;
	ULONG c      = 0x30;

	ret = XcModExp(&output, &a, &b, &c, 4/sizeof(ULONG));
	if(ret == 1 && output == 0x10) {
        test_passed &= 1;
	}
    else {
        test_passed &= 0;
	}

	// Passing a size of zero freezes (crash?) my xbox so I won't test it

	TEST_END();
}
