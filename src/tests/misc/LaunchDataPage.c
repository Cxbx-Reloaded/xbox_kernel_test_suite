#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(LaunchDataPage)
{
    TEST_BEGIN();

    // LaunchDataPage is a pointer to LAUNCH_DATA_PAGE
    // It may be NULL if no launch data was passed, or valid if launched from dashboard
    if (LaunchDataPage != NULL) {
        BOOLEAN is_valid = MmIsAddressValid(LaunchDataPage);
        GEN_CHECK(is_valid, TRUE, "LaunchDataPage address valid");
    } else {
        // NULL is valid - means no launch data
        GEN_CHECK(TRUE, TRUE, "LaunchDataPage is NULL (no launch data)");
    }

    TEST_END();
}
