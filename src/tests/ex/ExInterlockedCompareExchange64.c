#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ExInterlockedCompareExchange64)
{
    TEST_BEGIN();

    LONGLONG destination;
    LONGLONG exchange;
    LONGLONG comparand;
    LONGLONG result;

    // --- Successful exchange (comparand matches) ---
    destination = 100;
    exchange = 200;
    comparand = 100;
    result = ExInterlockedCompareExchange64(&destination, &exchange, &comparand);
    GEN_CHECK(result, 100, "returns old value on match");
    GEN_CHECK(destination, 200, "exchanged to 200");

    // --- Failed exchange (comparand doesn't match) ---
    comparand = 999;
    exchange = 300;
    result = ExInterlockedCompareExchange64(&destination, &exchange, &comparand);
    GEN_CHECK(result, 200, "returns current on mismatch");
    GEN_CHECK(destination, 200, "value unchanged on mismatch");

    // --- Exchange with zero ---
    destination = 42;
    exchange = 0;
    comparand = 42;
    result = ExInterlockedCompareExchange64(&destination, &exchange, &comparand);
    GEN_CHECK(result, 42, "old value 42");
    GEN_CHECK(destination, 0, "set to zero");

    // --- Exchange with negative values ---
    destination = -1;
    exchange = -9999;
    comparand = -1;
    result = ExInterlockedCompareExchange64(&destination, &exchange, &comparand);
    GEN_CHECK(result, -1, "old negative value");
    GEN_CHECK(destination, -9999, "exchanged to negative");

    // --- 64-bit boundary values ---
    destination = 0x7FFFFFFFFFFFFFFF; // LONGLONG_MAX
    exchange = 0;
    comparand = 0x7FFFFFFFFFFFFFFF;
    result = ExInterlockedCompareExchange64(&destination, &exchange, &comparand);
    GEN_CHECK(result, (LONGLONG)0x7FFFFFFFFFFFFFFF, "max value compared");
    GEN_CHECK(destination, 0, "exchanged from max");

    // --- Verify atomicity: comparand is the old value, not the new ---
    destination = 500;
    exchange = 600;
    comparand = 500;
    result = ExInterlockedCompareExchange64(&destination, &exchange, &comparand);
    GEN_CHECK(result, 500, "atomic: returns initial");
    GEN_CHECK(destination, 600, "atomic: swapped to 600");
    // Second attempt with stale comparand should fail
    comparand = 500; // stale
    exchange = 700;
    result = ExInterlockedCompareExchange64(&destination, &exchange, &comparand);
    GEN_CHECK(result, 600, "stale comparand sees current");
    GEN_CHECK(destination, 600, "not exchanged with stale comparand");

    TEST_END();
}
