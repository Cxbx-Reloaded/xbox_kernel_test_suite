#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(WRITE_PORT_BUFFER_USHORT)
{
    TEST_BEGIN();

    // Port 0x80 for POST diagnostics
    USHORT val = 0x00;
    WRITE_PORT_BUFFER_USHORT((PUSHORT)0x80, &val, 1);
    GEN_CHECK(TRUE, TRUE, "WRITE_PORT_BUFFER_USHORT no crash");

    TEST_END();
}
