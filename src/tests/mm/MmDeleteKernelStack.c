#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmDeleteKernelStack)
{
    TEST_BEGIN();

    // --- Create and delete: no crash ---
    PVOID stack = MmCreateKernelStack(0, FALSE);
    GEN_CHECK(stack != NULL, TRUE, "stack for delete");
    if (stack) {
        // Write to the stack to prove it was valid
        *((volatile ULONG*)((PUCHAR)stack - 4)) = 0xDEADC0DE;
        MmDeleteKernelStack(stack, NULL);
        GEN_CHECK(TRUE, TRUE, "delete completed");
    }

    // --- Delete multiple stacks in reverse order ---
    PVOID s1 = MmCreateKernelStack(0, FALSE);
    PVOID s2 = MmCreateKernelStack(0, FALSE);
    PVOID s3 = MmCreateKernelStack(0, FALSE);
    if (s3) MmDeleteKernelStack(s3, NULL);
    if (s2) MmDeleteKernelStack(s2, NULL);
    if (s1) MmDeleteKernelStack(s1, NULL);
    GEN_CHECK(TRUE, TRUE, "reverse delete order");

    // --- Delete in creation order ---
    s1 = MmCreateKernelStack(0, FALSE);
    s2 = MmCreateKernelStack(0, FALSE);
    if (s1) MmDeleteKernelStack(s1, NULL);
    if (s2) MmDeleteKernelStack(s2, NULL);
    GEN_CHECK(TRUE, TRUE, "forward delete order");

    // --- Create after delete (recycling) ---
    stack = MmCreateKernelStack(0, FALSE);
    if (stack) {
        MmDeleteKernelStack(stack, NULL);
        PVOID stack2 = MmCreateKernelStack(0, FALSE);
        GEN_CHECK(stack2 != NULL, TRUE, "create after delete works");
        if (stack2) MmDeleteKernelStack(stack2, NULL);
    }

    // --- NULL pointer ---
    // SKIP: MmDeleteKernelStack(NULL) likely dereferences the pointer to
    // find the base address, causing an access violation.

    TEST_END();
}
