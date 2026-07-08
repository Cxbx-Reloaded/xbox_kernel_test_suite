#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(IoInitializeIrp)
{
    TEST_BEGIN();

    // Allocate an IRP, then reinitialize it with different parameters
    PIRP irp = IoAllocateIrp(1);
    GEN_CHECK(irp != NULL, TRUE, "IoAllocateIrp non-NULL");
    if (irp) {
        // Dirty some fields
        irp->Flags = 0xDEAD;
        irp->Cancel = TRUE;

        // Reinitialize with stack size 2
        USHORT packet_size = irp->Size;
        IoInitializeIrp(irp, packet_size, 2);

        GEN_CHECK(irp->StackCount, 2, "StackCount after reinit");
        GEN_CHECK(irp->CurrentLocation, 3, "CurrentLocation after reinit");
        GEN_CHECK(irp->Flags, 0, "Flags cleared after reinit");
        GEN_CHECK(irp->Cancel, FALSE, "Cancel cleared after reinit");

        IoFreeIrp(irp);
    }

    TEST_END();
}
