#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KiBugCheckData)
{
    TEST_BEGIN();

    // KiBugCheckData is an exported array of 5 ULONGs
    // Since we haven't bugchecked, all entries should be 0
    GEN_CHECK(KiBugCheckData[0], 0, "BugCheckCode");
    GEN_CHECK(KiBugCheckData[1], 0, "BugCheckParameter1");
    GEN_CHECK(KiBugCheckData[2], 0, "BugCheckParameter2");
    GEN_CHECK(KiBugCheckData[3], 0, "BugCheckParameter3");
    GEN_CHECK(KiBugCheckData[4], 0, "BugCheckParameter4");

    TEST_END();
}
