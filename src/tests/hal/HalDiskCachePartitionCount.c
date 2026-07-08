#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(HalDiskCachePartitionCount)
{
    TEST_BEGIN();

    // HalDiskCachePartitionCount is an exported ULONG
    // Typically 0-3 cache partitions on Xbox
    GEN_CHECK(HalDiskCachePartitionCount <= 3, TRUE, "reasonable partition count");

    TEST_END();
}
