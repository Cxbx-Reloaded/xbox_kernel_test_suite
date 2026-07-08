#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ExInterlockedAddLargeInteger)
{
    TEST_BEGIN();

    LARGE_INTEGER addend;
    LARGE_INTEGER increment;

    // --- Basic positive addition ---
    addend.QuadPart = 100;
    increment.QuadPart = 50;
    LARGE_INTEGER result = ExInterlockedAddLargeInteger(&addend, increment);
    GEN_CHECK(result.QuadPart, 100, "returns old value (100)");
    GEN_CHECK(addend.QuadPart, 150, "addend updated to 150");

    // --- Negative addition (subtraction) ---
    increment.QuadPart = -30;
    result = ExInterlockedAddLargeInteger(&addend, increment);
    GEN_CHECK(result.QuadPart, 150, "returns old value (150)");
    GEN_CHECK(addend.QuadPart, 120, "addend updated to 120");

    // --- Add zero (no-op) ---
    increment.QuadPart = 0;
    result = ExInterlockedAddLargeInteger(&addend, increment);
    GEN_CHECK(result.QuadPart, 120, "returns old value for zero add");
    GEN_CHECK(addend.QuadPart, 120, "addend unchanged after zero add");

    // --- Carry from low 32 bits to high 32 bits ---
    addend.QuadPart = 0x00000000FFFFFFFE;
    increment.QuadPart = 3;
    result = ExInterlockedAddLargeInteger(&addend, increment);
    GEN_CHECK(result.QuadPart, (LONGLONG)0x00000000FFFFFFFE, "old value before carry");
    GEN_CHECK(addend.QuadPart, (LONGLONG)0x0000000100000001, "carry into high word");

    // --- Large values near LONGLONG max ---
    addend.QuadPart = 0x7FFFFFFFFFFFFFFE;
    increment.QuadPart = 1;
    result = ExInterlockedAddLargeInteger(&addend, increment);
    GEN_CHECK(result.QuadPart, (LONGLONG)0x7FFFFFFFFFFFFFFE, "old near-max");
    GEN_CHECK(addend.QuadPart, (LONGLONG)0x7FFFFFFFFFFFFFFF, "addend at LONGLONG_MAX");

    // --- Overflow wraps (signed overflow is defined for interlocked ops) ---
    increment.QuadPart = 1;
    result = ExInterlockedAddLargeInteger(&addend, increment);
    GEN_CHECK(result.QuadPart, (LONGLONG)0x7FFFFFFFFFFFFFFF, "old at max");
    // Wraps to negative (LONGLONG_MIN)
    GEN_CHECK(addend.QuadPart, (LONGLONG)0x8000000000000000, "overflow wraps to min");

    // --- Subtract to negative ---
    addend.QuadPart = 0;
    increment.QuadPart = -1;
    result = ExInterlockedAddLargeInteger(&addend, increment);
    GEN_CHECK(result.QuadPart, 0, "old is 0");
    GEN_CHECK(addend.QuadPart, -1, "subtract to -1");

    TEST_END();
}
