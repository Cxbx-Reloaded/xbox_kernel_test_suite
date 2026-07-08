#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(READ_PORT_BUFFER_ULONG)
{
    TEST_BEGIN();

    // Read from PCI config space port (0xCF8) - should be safe
    ULONG buf[1] = {0};
    READ_PORT_BUFFER_ULONG((PULONG)0xCF8, buf, 1);
    GEN_CHECK(TRUE, TRUE, "READ_PORT_BUFFER_ULONG no crash");

    TEST_END();
}
