#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(XcCryptService)
{
	TEST_BEGIN();

	ULONG ret;
	UCHAR test_buffer[8] = { 0 };

	ret = XcCryptService(0, NULL);
	GEN_CHECK(ret, 0, "XcCryptService return");

	ret = XcCryptService(0xFFFFFFFF, NULL);
	GEN_CHECK(ret, 0, "XcCryptService return");

	ret = XcCryptService(0xFFFFFFFF, test_buffer);
	GEN_CHECK(ret, 0, "XcCryptService return");
	GEN_CHECK_ARRAY(test_buffer, "\x00\x00\x00\x00\x00\x00\x00\x00", 8, "test_buffer"); // TODO: Do we need this?

	ret = XcCryptService(0, test_buffer);
	GEN_CHECK(ret, 0, "XcCryptService return");
	GEN_CHECK_ARRAY(test_buffer, "\x00\x00\x00\x00\x00\x00\x00\x00", 8, "test_buffer"); // TODO: Do we need this?

	TEST_END();
}
