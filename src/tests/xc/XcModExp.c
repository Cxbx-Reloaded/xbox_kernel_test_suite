#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(XcModExp)
{
    TEST_BEGIN();

	ULONG ret = 0;
	ULONG output = 0;
	ULONG a      = 0x10;
	ULONG b      = 0x20;
	ULONG c      = 0x30;

	ret = XcModExp(&output, &a, &b, &c, 4/sizeof(ULONG));
	GEN_CHECK(ret, 1, "XcModExp return");
	GEN_CHECK(output, 0x10, "output");

	// Passing a size of zero freezes (crash?) my xbox so I won't test it

	TEST_END();
}
