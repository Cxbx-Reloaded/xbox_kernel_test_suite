#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

// SMBus addresses on Xbox
#define SMC_ADDRESS         0x20  // System Management Controller
#define EEPROM_ADDRESS      0xA8  // EEPROM (I2C address)

TEST_FUNC(HalReadSMBusValue)
{
    TEST_BEGIN();

    typedef struct {
        const char* description;
        UCHAR slave_address;
        UCHAR command_code;
        BOOLEAN word_flag;
        NTSTATUS expected_status;
        ULONG max_value;  // upper bound for range validation
    } smbus_read_entry;

    smbus_read_entry read_tests[] = {
        // Valid reads from System Management Controller
        { "SMC revision",     SMC_ADDRESS, 0x01, FALSE, STATUS_SUCCESS, 0xFF },
        { "SMC tray state",   SMC_ADDRESS, 0x03, FALSE, STATUS_SUCCESS, 0xFF },
        { "SMC AV pack",      SMC_ADDRESS, 0x04, FALSE, STATUS_SUCCESS, 0xFF },
        { "SMC fan speed",    SMC_ADDRESS, 0x06, FALSE, STATUS_SUCCESS, 0xFF },
        { "SMC CPU temp",     SMC_ADDRESS, 0x09, FALSE, STATUS_SUCCESS, 0xFF },
        { "SMC board temp",   SMC_ADDRESS, 0x0A, FALSE, STATUS_SUCCESS, 0xFF },
        // EEPROM word read
        { "EEPROM word",      EEPROM_ADDRESS, 0x00, TRUE, STATUS_SUCCESS, 0xFFFF },
    };
    size_t num_reads = ARRAY_SIZE(read_tests);

    for (unsigned i = 0; i < num_reads; i++) {
        ULONG value = 0;
        NTSTATUS status = HalReadSMBusValue(
            read_tests[i].slave_address,
            read_tests[i].command_code,
            read_tests[i].word_flag,
            &value);
        GEN_CHECK(status, read_tests[i].expected_status, read_tests[i].description);
        GEN_CHECK(value <= read_tests[i].max_value, TRUE, read_tests[i].description);
    }

    // --- Invalid SMBus addresses (no device responds) ---
    typedef struct {
        const char* description;
        UCHAR slave_address;
    } smbus_invalid_entry;

    smbus_invalid_entry invalid_tests[] = {
        { "addr 0xFE (nothing)", 0xFE },
        { "addr 0x02 (nothing)", 0x02 },
    };
    size_t num_invalid = ARRAY_SIZE(invalid_tests);

    for (unsigned i = 0; i < num_invalid; i++) {
        ULONG value = 0xDEAD;
        NTSTATUS status = HalReadSMBusValue(invalid_tests[i].slave_address, 0x00, FALSE, &value);
        GEN_CHECK(status != STATUS_SUCCESS, TRUE, invalid_tests[i].description);
    }

    // --- Consistency: reading same register twice ---
    // Use AV pack register (0x04) which is stable, not version register
    // (0x01) which intentionally cycles through the 3-char version string.
    ULONG av1 = 0, av2 = 0;
    HalReadSMBusValue(SMC_ADDRESS, 0x04, FALSE, &av1);
    HalReadSMBusValue(SMC_ADDRESS, 0x04, FALSE, &av2);
    GEN_CHECK(av1, av2, "consistent reads");

    // --- NULL value pointer ---
    // SKIP: Passing NULL as the value pointer would cause HalReadSMBusValue to
    // write the result to address 0, triggering an access violation. The
    // function does not validate the pointer parameter.

    TEST_END();
}
