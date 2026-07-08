#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeReleaseMutant)
{
    TEST_BEGIN();

    KMUTANT mutant;

    // --- Basic acquire and release ---
    KeInitializeMutant(&mutant, TRUE); // owned
    LONG prev = KeReleaseMutant(&mutant, IO_NO_INCREMENT, FALSE, FALSE);
    GEN_CHECK(prev, 0, "was owned (SignalState=0)");
    GEN_CHECK(mutant.Header.SignalState, 1, "released = signaled");

    // --- Acquire via wait, then release ---
    KeInitializeMutant(&mutant, FALSE);
    LARGE_INTEGER zero = {0};
    KeWaitForSingleObject(&mutant, Executive, KernelMode, FALSE, &zero);
    GEN_CHECK(mutant.Header.SignalState, 0, "acquired via wait");

    prev = KeReleaseMutant(&mutant, IO_NO_INCREMENT, FALSE, FALSE);
    GEN_CHECK(prev, 0, "release after wait-acquire");
    GEN_CHECK(mutant.Header.SignalState, 1, "signaled after release");

    // --- Recursive acquisition and release ---
    KeInitializeMutant(&mutant, TRUE);
    // Recursively acquire
    KeWaitForSingleObject(&mutant, Executive, KernelMode, FALSE, &zero);
    GEN_CHECK(mutant.Header.SignalState, -1, "recursion level -1");
    KeWaitForSingleObject(&mutant, Executive, KernelMode, FALSE, &zero);
    GEN_CHECK(mutant.Header.SignalState, -2, "recursion level -2");

    // Release 3 times to fully free
    prev = KeReleaseMutant(&mutant, IO_NO_INCREMENT, FALSE, FALSE);
    GEN_CHECK(prev, -2, "prev was -2");
    GEN_CHECK(mutant.Header.SignalState, -1, "back to -1");

    prev = KeReleaseMutant(&mutant, IO_NO_INCREMENT, FALSE, FALSE);
    GEN_CHECK(prev, -1, "prev was -1");
    GEN_CHECK(mutant.Header.SignalState, 0, "back to 0");

    prev = KeReleaseMutant(&mutant, IO_NO_INCREMENT, FALSE, FALSE);
    GEN_CHECK(prev, 0, "prev was 0");
    GEN_CHECK(mutant.Header.SignalState, 1, "fully released");

    // --- Releasing an unowned mutant raises exception ---
    BOOLEAN caught = FALSE;
    __try {
        KeReleaseMutant(&mutant, IO_NO_INCREMENT, FALSE, FALSE);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        caught = TRUE;
    }
    GEN_CHECK(caught, TRUE, "releasing unowned mutant raises");

    TEST_END();
}
