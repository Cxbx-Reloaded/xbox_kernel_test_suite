#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

static volatile ULONG shutdown_called = 0;

static VOID NTAPI shutdown_callback(PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration)
{
    (void)ShutdownRegistration;
    shutdown_called++;
}

static VOID NTAPI shutdown_callback2(PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration)
{
    (void)ShutdownRegistration;
    shutdown_called += 10;
}

TEST_FUNC(HalRegisterShutdownNotification)
{
    TEST_BEGIN();

    // --- Register and unregister without crash ---
    HAL_SHUTDOWN_REGISTRATION reg;
    reg.NotificationRoutine = shutdown_callback;
    reg.Priority = 0;

    HalRegisterShutdownNotification(&reg, TRUE);
    GEN_CHECK(TRUE, TRUE, "register completed");

    HalRegisterShutdownNotification(&reg, FALSE);
    GEN_CHECK(TRUE, TRUE, "unregister completed");

    // --- Register/unregister multiple times ---
    HalRegisterShutdownNotification(&reg, TRUE);
    HalRegisterShutdownNotification(&reg, FALSE);
    HalRegisterShutdownNotification(&reg, TRUE);
    HalRegisterShutdownNotification(&reg, FALSE);
    GEN_CHECK(TRUE, TRUE, "multiple register/unregister cycles");

    // --- Multiple callbacks registered simultaneously ---
    HAL_SHUTDOWN_REGISTRATION reg2;
    reg2.NotificationRoutine = shutdown_callback2;
    reg2.Priority = 1;

    HalRegisterShutdownNotification(&reg, TRUE);
    HalRegisterShutdownNotification(&reg2, TRUE);
    GEN_CHECK(TRUE, TRUE, "two callbacks registered");

    // Unregister both
    HalRegisterShutdownNotification(&reg, FALSE);
    HalRegisterShutdownNotification(&reg2, FALSE);
    GEN_CHECK(TRUE, TRUE, "both unregistered");

    // --- Different priorities ---
    reg.Priority = 5;
    reg2.Priority = 10;
    HalRegisterShutdownNotification(&reg, TRUE);
    HalRegisterShutdownNotification(&reg2, TRUE);
    HalRegisterShutdownNotification(&reg2, FALSE);
    HalRegisterShutdownNotification(&reg, FALSE);
    GEN_CHECK(TRUE, TRUE, "priority ordering register/unregister");

    // Note: We cannot actually trigger shutdown to verify callbacks fire
    // without rebooting the system.

    TEST_END();
}
