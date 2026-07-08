#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(MmUnmapIoSpace)
{
    TEST_BEGIN();

    // Map and unmap - verify no crash
    PHYSICAL_ADDRESS phys_addr = 0xFD000000; // NV2A

    PVOID mapped = MmMapIoSpace(phys_addr, PAGE_SIZE, PAGE_READWRITE | PAGE_NOCACHE);
    GEN_CHECK(mapped != NULL, TRUE, "mapped for unmap test");
    if (mapped) {
        MmUnmapIoSpace(mapped, PAGE_SIZE);
        GEN_CHECK(TRUE, TRUE, "unmap completed");
    }

    TEST_END();
}
