#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(IoFreeIrp)
{
    TEST_BEGIN();

    // Allocate and free - verify no crash
    PIRP irp = IoAllocateIrp(1);
    GEN_CHECK(irp != NULL, TRUE, "IoAllocateIrp non-NULL");
    if (irp) {
        IoFreeIrp(irp);
    }

    // Allocate with larger stack size and free
    irp = IoAllocateIrp(3);
    GEN_CHECK(irp != NULL, TRUE, "IoAllocateIrp(3) non-NULL");
    if (irp) {
        IoFreeIrp(irp);
    }

    // Allocate again after free to verify pool is reusable
    irp = IoAllocateIrp(1);
    GEN_CHECK(irp != NULL, TRUE, "re-alloc after free non-NULL");
    if (irp) {
        IoFreeIrp(irp);
    }

    TEST_END();
}
