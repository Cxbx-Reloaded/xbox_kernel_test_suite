#include <xboxkrnl/xboxkrnl.h>
#include <stddef.h>

#include "defines.h"

BOOL assert_hashed_result(
    PUCHAR input,
    size_t len,
    PUCHAR expected_result,
    const char *test_name)
{
    ASSERT_HEADER
    unsigned char sha1_ctx[116] = {0};
    unsigned char digest[20] = {0};

    XcSHAInit(sha1_ctx);
    XcSHAUpdate(sha1_ctx, input, len);
    XcSHAFinal(sha1_ctx, digest);

    GEN_CHECK_ARRAY(digest, expected_result, 20, test_name)

    return test_passed;
}
