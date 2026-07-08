#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

static BOOLEAN NTAPI dummy_isr(PKINTERRUPT Interrupt)
{
    (void)Interrupt;
    return FALSE;
}

TEST_FUNC(KeInitializeInterrupt)
{
    TEST_BEGIN();

    KINTERRUPT interrupt;

    KeInitializeInterrupt(
        &interrupt,
        (PKSERVICE_ROUTINE)dummy_isr,
        NULL,       // ServiceContext
        14,         // Vector (use unused vector)
        DISPATCH_LEVEL,  // Irql
        LevelSensitive,  // InterruptMode
        FALSE       // ShareVector
    );

    GEN_CHECK(interrupt.ServiceRoutine == (PKSERVICE_ROUTINE)dummy_isr, TRUE, "ISR set");
    GEN_CHECK(interrupt.BusInterruptLevel, 14, "vector set");

    // Don't connect - just verify initialization
    TEST_END();
}
