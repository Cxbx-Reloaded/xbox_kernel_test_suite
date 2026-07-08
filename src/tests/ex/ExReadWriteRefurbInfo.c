#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ExReadWriteRefurbInfo)
{
    TEST_BEGIN();

    // Read refurb info
    XBOX_REFURB_INFO refurb_info;
    RtlZeroMemory(&refurb_info, sizeof(refurb_info));

    NTSTATUS status = ExReadWriteRefurbInfo(&refurb_info, sizeof(refurb_info), FALSE); // FALSE = read
    GEN_CHECK(status, STATUS_SUCCESS, "read refurb info");

    // Write it back unchanged (safe)
    status = ExReadWriteRefurbInfo(&refurb_info, sizeof(refurb_info), TRUE); // TRUE = write
    GEN_CHECK(status, STATUS_SUCCESS, "write refurb info back");

    TEST_END();
}
