#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(IoAllocateIrp)
{
    TEST_BEGIN();

    // Allocate IRP with stack size 1
    PIRP irp = IoAllocateIrp(1);
    GEN_CHECK(irp != NULL, TRUE, "IoAllocateIrp(1) non-NULL");
    if (irp) {
        GEN_CHECK(irp->StackCount, 1, "StackCount == 1");
        GEN_CHECK(irp->CurrentLocation, 2, "CurrentLocation == 2");
        GEN_CHECK(irp->Cancel, FALSE, "Cancel == FALSE");
        GEN_CHECK(irp->Flags, 0, "Flags == 0");
        IoFreeIrp(irp);
    }

    // Allocate IRP with stack size 3
    irp = IoAllocateIrp(3);
    GEN_CHECK(irp != NULL, TRUE, "IoAllocateIrp(3) non-NULL");
    if (irp) {
        GEN_CHECK(irp->StackCount, 3, "StackCount == 3");
        GEN_CHECK(irp->CurrentLocation, 4, "CurrentLocation == 4");
        IoFreeIrp(irp);
    }

    TEST_END();
}
