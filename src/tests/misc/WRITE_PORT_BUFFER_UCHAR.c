#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(WRITE_PORT_BUFFER_UCHAR)
{
    TEST_BEGIN();

    // Writing to arbitrary ports is dangerous
    // Just test that the function exists and is callable
    // We'll write to port 0x80 (POST code port) which is safe for diagnostics
    UCHAR val = 0xAA;
    WRITE_PORT_BUFFER_UCHAR((PUCHAR)0x80, &val, 1);
    GEN_CHECK(TRUE, TRUE, "WRITE_PORT_BUFFER_UCHAR no crash");

    TEST_END();
}
