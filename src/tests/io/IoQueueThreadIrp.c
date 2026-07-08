#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(IoQueueThreadIrp)
{
    TEST_BEGIN();

    // IoQueueThreadIrp queues an IRP to the current thread's IRP list.
    // This is used by the I/O manager to track outstanding IRPs per thread.
    PIRP irp = IoAllocateIrp(1);
    GEN_CHECK(irp != NULL, TRUE, "IRP allocated");
    if (irp) {
        // Queue it to the current thread
        IoQueueThreadIrp(irp);

        // The IRP's Tail.Overlay.Thread should now point to current thread
        GEN_CHECK((PKTHREAD)irp->Tail.Overlay.Thread == KeGetCurrentThread(), TRUE, "IRP thread set");

        // Clean up - need to dequeue before freeing
        // Remove from thread IRP list
        RemoveEntryList(&irp->ThreadListEntry);
        IoFreeIrp(irp);
    }

    TEST_END();
}
