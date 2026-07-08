#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(RtlMapGenericMask)
{
    TEST_BEGIN();

    // Set up a generic mapping (typical file object mapping)
    GENERIC_MAPPING mapping = {
        .GenericRead = FILE_GENERIC_READ,
        .GenericWrite = FILE_GENERIC_WRITE,
        .GenericExecute = FILE_GENERIC_EXECUTE,
        .GenericAll = FILE_ALL_ACCESS
    };

    // Test GENERIC_READ mapping
    ACCESS_MASK mask = GENERIC_READ;
    RtlMapGenericMask(&mask, &mapping);
    GEN_CHECK(mask, FILE_GENERIC_READ, "GENERIC_READ mapped");

    // Test GENERIC_WRITE mapping
    mask = GENERIC_WRITE;
    RtlMapGenericMask(&mask, &mapping);
    GEN_CHECK(mask, FILE_GENERIC_WRITE, "GENERIC_WRITE mapped");

    // Test GENERIC_EXECUTE mapping
    mask = GENERIC_EXECUTE;
    RtlMapGenericMask(&mask, &mapping);
    GEN_CHECK(mask, FILE_GENERIC_EXECUTE, "GENERIC_EXECUTE mapped");

    // Test GENERIC_ALL mapping
    mask = GENERIC_ALL;
    RtlMapGenericMask(&mask, &mapping);
    GEN_CHECK(mask, FILE_ALL_ACCESS, "GENERIC_ALL mapped");

    // Test combination: GENERIC_READ | GENERIC_WRITE
    mask = GENERIC_READ | GENERIC_WRITE;
    RtlMapGenericMask(&mask, &mapping);
    GEN_CHECK(mask, (FILE_GENERIC_READ | FILE_GENERIC_WRITE), "READ|WRITE mapped");

    // Non-generic bits should be preserved
    mask = GENERIC_READ | DELETE;
    RtlMapGenericMask(&mask, &mapping);
    GEN_CHECK(mask, (FILE_GENERIC_READ | DELETE), "generic + specific preserved");

    // No generic bits = no change
    mask = FILE_READ_DATA | FILE_WRITE_DATA;
    RtlMapGenericMask(&mask, &mapping);
    GEN_CHECK(mask, (FILE_READ_DATA | FILE_WRITE_DATA), "no generic bits unchanged");

    TEST_END();
}
