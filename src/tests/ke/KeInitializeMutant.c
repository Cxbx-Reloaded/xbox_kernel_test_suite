#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeInitializeMutant)
{
    TEST_BEGIN();

    KMUTANT mutant;

    // --- Initialize not owned: signaled, available ---
    memset(&mutant, 0xCC, sizeof(mutant));
    KeInitializeMutant(&mutant, FALSE);
    GEN_CHECK(mutant.Header.SignalState, 1, "not owned = signaled");
    GEN_CHECK(mutant.Header.Type, MutantObject, "type is MutantObject");
    GEN_CHECK(mutant.MutantListEntry.Flink == NULL || mutant.MutantListEntry.Flink == &mutant.MutantListEntry, TRUE, "list entry init");

    // Should be immediately acquirable via wait
    LARGE_INTEGER zero = {0};
    NTSTATUS status = KeWaitForSingleObject(&mutant, Executive, KernelMode, FALSE, &zero);
    GEN_CHECK(status, STATUS_SUCCESS, "unowned mutant acquired");
    GEN_CHECK(mutant.Header.SignalState, 0, "owned after wait");
    KeReleaseMutant(&mutant, IO_NO_INCREMENT, FALSE, FALSE);

    // --- Initialize owned: not signaled, owned by current thread ---
    KeInitializeMutant(&mutant, TRUE);
    GEN_CHECK(mutant.Header.SignalState, 0, "owned = not signaled");

    // Can still be recursively acquired by same thread
    status = KeWaitForSingleObject(&mutant, Executive, KernelMode, FALSE, &zero);
    GEN_CHECK(status, STATUS_SUCCESS, "recursive acquire");
    GEN_CHECK(mutant.Header.SignalState, -1, "recursion count = -1");

    // Release twice to fully release
    KeReleaseMutant(&mutant, IO_NO_INCREMENT, FALSE, FALSE);
    GEN_CHECK(mutant.Header.SignalState, 0, "released once (still owned)");
    KeReleaseMutant(&mutant, IO_NO_INCREMENT, FALSE, FALSE);
    GEN_CHECK(mutant.Header.SignalState, 1, "released twice (free)");

    // --- Two mutants are independent ---
    KMUTANT mut1, mut2;
    KeInitializeMutant(&mut1, FALSE);
    KeInitializeMutant(&mut2, TRUE);
    GEN_CHECK(mut1.Header.SignalState, 1, "mut1 free");
    GEN_CHECK(mut2.Header.SignalState, 0, "mut2 owned");
    KeReleaseMutant(&mut2, IO_NO_INCREMENT, FALSE, FALSE);

    TEST_END();
}
