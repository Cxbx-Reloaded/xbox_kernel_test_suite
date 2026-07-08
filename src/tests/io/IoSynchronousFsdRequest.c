#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(IoSynchronousFsdRequest)
{
    TEST_BEGIN();

    // SKIP: IoSynchronousFsdRequest builds and sends a synchronous FSD IRP
    // to a device object, waiting for completion. Sending to our dummy device
    // would invoke IoInvalidDeviceRequest which completes the IRP, but the
    // synchronous wait machinery expects a real device stack. This could
    // deadlock or crash if the device doesn't properly signal completion.

    TEST_SKIP("needs real device stack");

    TEST_END();
}
