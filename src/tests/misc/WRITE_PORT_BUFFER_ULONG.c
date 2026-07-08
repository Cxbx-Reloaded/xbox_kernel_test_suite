#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(WRITE_PORT_BUFFER_ULONG)
{
    TEST_BEGIN();

    // Port 0x80 extended to ULONG for POST diagnostics
    ULONG val = 0x00;
    WRITE_PORT_BUFFER_ULONG((PULONG)0x80, &val, 1);
    GEN_CHECK(TRUE, TRUE, "WRITE_PORT_BUFFER_ULONG no crash");

    TEST_END();
}
