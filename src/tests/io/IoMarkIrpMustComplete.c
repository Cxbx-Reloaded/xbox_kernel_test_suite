#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

#ifndef IRP_MUST_COMPLETE_REQUEST
#define IRP_MUST_COMPLETE_REQUEST 0x00002000
#endif

TEST_FUNC(IoMarkIrpMustComplete)
{
    TEST_BEGIN();

    PIRP irp = IoAllocateIrp(1);
    GEN_CHECK(irp != NULL, TRUE, "IoAllocateIrp non-NULL");
    if (irp) {
        // Verify flag is not set initially
        GEN_CHECK((irp->Flags & IRP_MUST_COMPLETE_REQUEST), 0, "flag not set initially");

        // Mark IRP must complete
        IoMarkIrpMustComplete(irp);

        // Verify flag is now set
        GEN_CHECK((irp->Flags & IRP_MUST_COMPLETE_REQUEST) != 0, TRUE, "flag set after mark");

        IoFreeIrp(irp);
    }

    TEST_END();
}
