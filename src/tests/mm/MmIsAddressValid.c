#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(MmIsAddressValid)
{
    TEST_BEGIN();

    // --- Valid addresses ---

    // Kernel code address (exported function pointer)
    BOOLEAN result = is_kernel_export_valid((PVOID)MmIsAddressValid);
    GEN_CHECK(result, TRUE, "kernel function address");

    // Another kernel function (cross-check it's not just one page)
    result = is_kernel_export_valid((PVOID)ExAllocatePool);
    GEN_CHECK(result, TRUE, "second kernel function");

    // Stack address
    volatile int stack_var = 42;
    result = MmIsAddressValid((PVOID)&stack_var);
    GEN_CHECK(result, TRUE, "stack address");

    // Global data (exported kernel variable)
    result = is_kernel_export_valid((PVOID)&KeTickCount);
    GEN_CHECK(result, TRUE, "kernel global data");

    // Dynamically allocated memory
    PVOID alloc = ExAllocatePool(64);
    if (alloc) {
        result = MmIsAddressValid(alloc);
        GEN_CHECK(result, TRUE, "pool-allocated memory");
        ExFreePool(alloc);
    }

    // --- Invalid addresses ---

    // NULL pointer
    result = MmIsAddressValid(NULL);
    GEN_CHECK(result, FALSE, "NULL address");

    // Low addresses (user-mode range, unmapped on Xbox)
    result = MmIsAddressValid((PVOID)0x00000001);
    GEN_CHECK(result, FALSE, "0x1 (low unmapped)");

    result = MmIsAddressValid((PVOID)0x00001000);
    GEN_CHECK(result, FALSE, "0x1000 (low unmapped)");

    result = MmIsAddressValid((PVOID)0x0000FFFF);
    GEN_CHECK(result, FALSE, "0xFFFF (low unmapped)");

    // Very high address (above physical memory mapping)
    result = MmIsAddressValid((PVOID)0xFFFFFFFF);
    GEN_CHECK(result, FALSE, "0xFFFFFFFF (high unmapped)");

    // Address in the middle of unmapped region
    result = MmIsAddressValid((PVOID)0x00100000);
    GEN_CHECK(result, FALSE, "0x100000 (low megabyte, may be unmapped)");

    // --- Boundary: page-aligned address just past mapped range ---
    // SKIP: We can't reliably determine the exact boundary of mapped memory
    // without querying page tables directly. The above tests cover the
    // meaningful cases: known-good and known-bad addresses.

    // --- Freed memory ---
    // Note: After ExFreePool, the virtual address may or may not remain valid
    // depending on whether the page is decommitted. We intentionally do NOT
    // test freed memory because the result is implementation-defined and
    // testing it could read from a reused allocation.

    TEST_END();
}
