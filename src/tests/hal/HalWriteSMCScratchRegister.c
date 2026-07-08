#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(HalWriteSMCScratchRegister)
{
    TEST_BEGIN();

    // HalWriteSMCScratchRegister writes to SMC scratch register
    // This is safe to call - just writes a value for diagnostic purposes
    HalWriteSMCScratchRegister(0x00);
    GEN_CHECK(TRUE, TRUE, "scratch register write no crash");

    TEST_END();
}
