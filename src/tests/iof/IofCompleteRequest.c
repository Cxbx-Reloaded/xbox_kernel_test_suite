#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(IofCompleteRequest)
{
    TEST_BEGIN();

    // IofCompleteRequest completes an IRP and signals any waiting threads.
    // We test by allocating an IRP, setting its status, and completing it.
    PIRP irp = IoAllocateIrp(1);
    GEN_CHECK(irp != NULL, TRUE, "IRP allocated");
    if (irp) {
        irp->IoStatus.Status = STATUS_SUCCESS;
        irp->IoStatus.Information = 0;

        // Complete the IRP with no priority boost
        IofCompleteRequest(irp, IO_NO_INCREMENT);

        // After completion, the IRP is freed by the I/O manager
        // so we don't call IoFreeIrp here
        GEN_CHECK(TRUE, TRUE, "IofCompleteRequest did not crash");
    }

    TEST_END();
}
