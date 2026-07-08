#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(READ_PORT_BUFFER_UCHAR)
{
    TEST_BEGIN();

    // Read from a safe I/O port (PIC status register, port 0x20)
    UCHAR buf[4] = {0};
    READ_PORT_BUFFER_UCHAR((PUCHAR)0x20, buf, 1);
    // Just verify it doesn't crash and returns something
    GEN_CHECK(TRUE, TRUE, "READ_PORT_BUFFER_UCHAR no crash");

    TEST_END();
}
