#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(HalDiskSerialNumber)
{
    TEST_BEGIN();

    // HalDiskSerialNumber is a STRING struct (not a pointer)
    GEN_CHECK(HalDiskSerialNumber.Length > 0, TRUE, "length > 0");
    GEN_CHECK(HalDiskSerialNumber.MaximumLength >= HalDiskSerialNumber.Length, TRUE, "maxlen >= len");
    if (HalDiskSerialNumber.Buffer) {
        BOOLEAN is_valid = MmIsAddressValid(HalDiskSerialNumber.Buffer);
        GEN_CHECK(is_valid, TRUE, "buffer valid");
    }

    TEST_END();
}
