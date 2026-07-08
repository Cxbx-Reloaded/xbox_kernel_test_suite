#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(DbgUnLoadImageSymbols)
{
    TEST_BEGIN();

    // DbgUnLoadImageSymbols notifies the kernel debugger about unloaded symbols.
    // It's a no-op when no debugger is attached. No observable side effect to verify.

    TEST_SKIP("debugger notification only, no observable effect");

    TEST_END();
}
