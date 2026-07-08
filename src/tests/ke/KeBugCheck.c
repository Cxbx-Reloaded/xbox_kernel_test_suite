#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeBugCheck)
{
    TEST_BEGIN();

    // KeBugCheck would crash the system - DO NOT CALL

    TEST_SKIP("would BSOD");

    TEST_END();
}
