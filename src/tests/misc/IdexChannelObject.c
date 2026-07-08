#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(IdexChannelObject)
{
    TEST_BEGIN();

    // IdexChannelObject is an exported IDE_CHANNEL_OBJECT struct
    // Verify its address is in valid kernel memory
    BOOLEAN is_valid = is_kernel_export_valid(&IdexChannelObject);
    GEN_CHECK(is_valid, TRUE, "IdexChannelObject address valid");

    TEST_END();
}
