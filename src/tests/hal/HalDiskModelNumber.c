#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(HalDiskModelNumber)
{
    TEST_BEGIN();

    // HalDiskModelNumber is a STRING struct (not a pointer)
    // Length should be > 0 (some model string exists)
    GEN_CHECK(HalDiskModelNumber.Length > 0, TRUE, "length > 0");
    // MaximumLength should be >= Length
    GEN_CHECK(HalDiskModelNumber.MaximumLength >= HalDiskModelNumber.Length, TRUE, "maxlen >= len");
    // Buffer should be valid
    if (HalDiskModelNumber.Buffer) {
        BOOLEAN is_valid = MmIsAddressValid(HalDiskModelNumber.Buffer);
        GEN_CHECK(is_valid, TRUE, "buffer valid");
    }

    TEST_END();
}
