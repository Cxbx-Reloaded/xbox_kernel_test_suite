#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(DbgBreakPoint)
{
    TEST_BEGIN();

    // DbgBreakPoint triggers an INT 3 breakpoint exception.
    // Calling this without a kernel debugger attached will crash the system.
    // Cannot be tested safely in an automated test environment.

    TEST_SKIP("would trigger INT 3");

    TEST_END();
}
