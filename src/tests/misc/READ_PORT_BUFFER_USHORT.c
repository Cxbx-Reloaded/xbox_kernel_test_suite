#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(READ_PORT_BUFFER_USHORT)
{
    TEST_BEGIN();

    // Read from a safe I/O port
    USHORT buf[1] = {0};
    READ_PORT_BUFFER_USHORT((PUSHORT)0x20, buf, 1);
    GEN_CHECK(TRUE, TRUE, "READ_PORT_BUFFER_USHORT no crash");

    TEST_END();
}
