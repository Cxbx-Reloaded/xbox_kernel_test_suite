#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeTimeIncrement)
{
    TEST_BEGIN();

    // KeTimeIncrement is the number of 100ns units per clock tick
    // On Xbox it should be ~10000 (1ms per tick) or similar value
    ULONG time_increment = KeTimeIncrement;
    GEN_CHECK(time_increment != 0, TRUE, "time increment non-zero");

    // Should be a reasonable value (between 1000 and 200000 hundred-ns units)
    // i.e., between 0.1ms and 20ms per tick
    GEN_CHECK(time_increment >= 1000, TRUE, "time increment >= 0.1ms");
    GEN_CHECK(time_increment <= 200000, TRUE, "time increment <= 20ms");

    TEST_END();
}
