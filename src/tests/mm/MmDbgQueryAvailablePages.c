#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmDbgQueryAvailablePages)
{
    TEST_BEGIN();

    // DEVKIT-only. On retail, the kernel thunk pointer is NULL.
    if ((void*)MmDbgQueryAvailablePages == NULL) {
        TEST_SKIP("thunk is NULL (retail)");
        TEST_END();
        return;
    }

    ULONG pages = MmDbgQueryAvailablePages();
    GEN_CHECK(pages >= 0, TRUE, "non-negative pages");

    TEST_END();
}
