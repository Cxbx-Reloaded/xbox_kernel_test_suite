#include "av_tests.h"

void (*kernel_thunk_table[])(void) =
{
	NULL,                                     // 0x0000 (0)   NULL
	test_AvGetSavedDataAddress,               // 0x0001 (1)
	test_AvSendTVEncoderOption,               // 0x0002 (2)
	test_AvSetDisplayMode,                    // 0x0003 (3)
	test_AvSetSavedDataAddress,               // 0x0004 (4)
};
