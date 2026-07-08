#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(IoSynchronousDeviceIoControlRequest)
{
    TEST_BEGIN();

    // SKIP: IoSynchronousDeviceIoControlRequest builds and sends a synchronous
    // IOCTL to a device, waiting for completion. Same concerns as
    // IoSynchronousFsdRequest - could deadlock with our dummy device.

    TEST_SKIP("needs real device stack");

    TEST_END();
}
