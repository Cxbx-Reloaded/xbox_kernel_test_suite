#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(DbgBreakPointWithStatus)
{
    TEST_BEGIN();

    // DbgBreakPointWithStatus triggers an INT 3 with a status code.
    // Same as DbgBreakPoint - will crash without a debugger attached.

    TEST_SKIP("would trigger INT 3");

    TEST_END();
}
