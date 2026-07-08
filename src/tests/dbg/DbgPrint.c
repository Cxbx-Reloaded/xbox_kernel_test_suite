#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(DbgPrint)
{
    TEST_BEGIN();

    // DbgPrint outputs a formatted string to the kernel debugger.
    // Returns STATUS_SUCCESS on success.
    ULONG status = DbgPrint("xbox_kernel_test_suite: DbgPrint test - %d %s\n", 42, "hello");
    GEN_CHECK(status, STATUS_SUCCESS, "DbgPrint returns SUCCESS");

    // Empty string
    status = DbgPrint("");
    GEN_CHECK(status, STATUS_SUCCESS, "DbgPrint empty string");

    // Format specifiers
    status = DbgPrint("%u %x %p\n", 123, 0xDEAD, (void*)0x12345678);
    GEN_CHECK(status, STATUS_SUCCESS, "DbgPrint format specifiers");

    TEST_END();
}
