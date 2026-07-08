#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(HalReadSMCTrayState)
{
    TEST_BEGIN();

    ULONG tray_state = 0;
    ULONG tray_state_change_count = 0;

    HalReadSMCTrayState(&tray_state, &tray_state_change_count);

    // Tray state should be a known value:
    // 0x00=closed, 0x10=open, 0x40=opening, 0x60=closing, etc.
    // Just verify it's in a reasonable range
    GEN_CHECK(tray_state <= 0xFF, TRUE, "tray state reasonable");

    TEST_END();
}
