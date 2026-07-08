#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(UnknownAPI367)
{
    TEST_BEGIN();

    // SKIP: This kernel export has no known documentation or signature.
    // Cannot be tested without knowing what the function does or its parameters.

    TEST_SKIP("undocumented kernel export");

    TEST_END();
}
