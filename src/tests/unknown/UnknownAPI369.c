#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(UnknownAPI369)
{
    TEST_BEGIN();

    // SKIP: This kernel export has no known documentation or signature.

    TEST_SKIP("undocumented kernel export");

    TEST_END();
}
