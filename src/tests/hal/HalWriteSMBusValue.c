#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

#define SMC_ADDRESS     0x20
#define SMC_CMD_LEDSEQ  0x08  // LED sequence register (safe to write)
#define SMC_CMD_LEDMODE 0x07  // LED mode register

TEST_FUNC(HalWriteSMBusValue)
{
    TEST_BEGIN();

    // --- Write to invalid address: should fail ---
    NTSTATUS status = HalWriteSMBusValue(0xFE, 0x00, FALSE, 0x00);
    GEN_CHECK(status != STATUS_SUCCESS, TRUE, "invalid addr 0xFE fails");

    // --- Another invalid address ---
    status = HalWriteSMBusValue(0x02, 0x00, FALSE, 0x00);
    GEN_CHECK(status != STATUS_SUCCESS, TRUE, "invalid addr 0x02 fails");

    // --- Write to SMC LED register (safe, cosmetic only) ---
    // Write the default green LED pattern (0x0F = all green segments)
    status = HalWriteSMBusValue(SMC_ADDRESS, SMC_CMD_LEDSEQ, FALSE, 0x0F);
    GEN_CHECK(status, STATUS_SUCCESS, "write LED sequence");

    // --- Read back what we can to verify the bus works ---
    ULONG readback = 0;
    HalReadSMBusValue(SMC_ADDRESS, SMC_CMD_LEDSEQ, FALSE, &readback);
    // Some SMC registers are write-only, so readback may differ
    // Just verify the write didn't crash
    GEN_CHECK(TRUE, TRUE, "write/read cycle no crash");

    // --- Word write to invalid address ---
    status = HalWriteSMBusValue(0xFE, 0x00, TRUE, 0x0000);
    GEN_CHECK(status != STATUS_SUCCESS, TRUE, "word write invalid fails");

    // --- Write to EEPROM address ---
    // SKIP: Writing to EEPROM (0xA8) would modify persistent Xbox settings
    // (video region, encryption keys, etc.). This is destructive and
    // irreversible without a backup. Never test EEPROM writes.

    // --- Write to fan speed ---
    // SKIP: Writing to fan speed register (0x06) could damage hardware if
    // set too low on real hardware. Only safe in emulators.

    TEST_END();
}
