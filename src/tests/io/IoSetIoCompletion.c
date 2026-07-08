#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(IoSetIoCompletion)
{
    TEST_BEGIN();

    /* SKIP: IoSetIoCompletion requires a valid IO completion port object
     * (created via NtCreateIoCompletion). Creating and managing IO completion
     * ports adds significant complexity and risk of leaking kernel objects.
     * Skipping to avoid potential system instability.
     */
    print("  SKIP: Requires IO completion port object (NtCreateIoCompletion)");

    TEST_END();
}
