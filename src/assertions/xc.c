#include <xboxkrnl/xboxkrnl.h>
#include <stddef.h>

#include "defines.h"

BOOL assert_hashed_result_ex(
    PUCHAR input,
    size_t len,
    PUCHAR expected_result,
    const char *test_name,
    int line_number)
{
    ASSERT_HEADER
    unsigned char sha1_ctx[116] = {0};
    unsigned char digest[20] = {0};

    XcSHAInit(sha1_ctx);
    XcSHAUpdate(sha1_ctx, input, len);
    XcSHAFinal(sha1_ctx, digest);

    GEN_CHECK_ARRAY_EX(digest, expected_result, 20, test_name, line_number)

    return test_passed;
}
