#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

// Known physical addresses on Xbox hardware
#define NV2A_MMIO_BASE_PHYS   0xFD000000  // NV2A GPU registers
#define NV2A_PMC_BOOT_0       0x000000    // PMC.BOOT_0 offset (device ID)
#define APU_BASE_PHYS         0xFE800000  // MCPX APU
#define NIC_BASE_PHYS         0xFEF00000  // Network controller

TEST_FUNC(MmMapIoSpace)
{
    TEST_BEGIN();

    PHYSICAL_ADDRESS phys_addr;

    // --- Map NV2A GPU registers (always present) ---
    phys_addr = NV2A_MMIO_BASE_PHYS;
    PVOID mapped = MmMapIoSpace(phys_addr, PAGE_SIZE, PAGE_READWRITE | PAGE_NOCACHE);
    GEN_CHECK(mapped != NULL, TRUE, "map NV2A MMIO");
    if (mapped) {
        GEN_CHECK(MmIsAddressValid(mapped), TRUE, "mapped address valid");
        // Read PMC.BOOT_0 - contains NV2A device ID
        volatile ULONG boot0 = *(volatile ULONG*)mapped;
        // NV2A BOOT_0 should have a valid NVidia device identifier
        GEN_CHECK(boot0 != 0, TRUE, "GPU BOOT_0 non-zero");
        GEN_CHECK(boot0 != 0xFFFFFFFF, TRUE, "GPU BOOT_0 not all-ones");
        MmUnmapIoSpace(mapped, PAGE_SIZE);
    }

    // --- Map larger region ---
    phys_addr = NV2A_MMIO_BASE_PHYS;
    mapped = MmMapIoSpace(phys_addr, PAGE_SIZE * 4, PAGE_READWRITE | PAGE_NOCACHE);
    GEN_CHECK(mapped != NULL, TRUE, "map 4 pages");
    if (mapped) {
        // All 4 pages should be valid
        GEN_CHECK(MmIsAddressValid(mapped), TRUE, "page 0 valid");
        GEN_CHECK(MmIsAddressValid((PUCHAR)mapped + PAGE_SIZE), TRUE, "page 1 valid");
        GEN_CHECK(MmIsAddressValid((PUCHAR)mapped + PAGE_SIZE * 3), TRUE, "page 3 valid");
        MmUnmapIoSpace(mapped, PAGE_SIZE * 4);
    }

    // --- Map APU (MCPX) registers ---
    phys_addr = APU_BASE_PHYS;
    mapped = MmMapIoSpace(phys_addr, PAGE_SIZE, PAGE_READWRITE | PAGE_NOCACHE);
    GEN_CHECK(mapped != NULL, TRUE, "map APU");
    if (mapped) {
        GEN_CHECK(MmIsAddressValid(mapped), TRUE, "APU mapped valid");
        MmUnmapIoSpace(mapped, PAGE_SIZE);
    }

    // --- Two mappings of same physical address may differ in virtual ---
    phys_addr = NV2A_MMIO_BASE_PHYS;
    PVOID map1 = MmMapIoSpace(phys_addr, PAGE_SIZE, PAGE_READWRITE | PAGE_NOCACHE);
    PVOID map2 = MmMapIoSpace(phys_addr, PAGE_SIZE, PAGE_READWRITE | PAGE_NOCACHE);
    if (map1 && map2) {
        // Both should read the same hardware value
        volatile ULONG v1 = *(volatile ULONG*)map1;
        volatile ULONG v2 = *(volatile ULONG*)map2;
        GEN_CHECK(v1, v2, "same phys addr = same value");
    }
    if (map1) MmUnmapIoSpace(map1, PAGE_SIZE);
    if (map2) MmUnmapIoSpace(map2, PAGE_SIZE);

    // --- Size 0 ---
    // SKIP: MmMapIoSpace with size 0 is implementation-defined.
    // May return NULL or a valid mapping. Not worth testing.

    TEST_END();
}
