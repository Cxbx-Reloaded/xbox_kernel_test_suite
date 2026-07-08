#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

// PCI config space offsets
#define PCI_VENDOR_ID    0x00
#define PCI_DEVICE_ID    0x02
#define PCI_COMMAND      0x04
#define PCI_CLASS_REV    0x08
#define PCI_HEADER_TYPE  0x0E

// Xbox PCI topology:
// Bus 0, Dev 0: Host bridge (NVIDIA nForce)
// Bus 0, Dev 1: ISA bridge (MCPX)
// Bus 0, Dev 2: USB OHCI #0
// Bus 0, Dev 3: USB OHCI #1
// Bus 0, Dev 4: Network (nvnet)
// Bus 0, Dev 5: APU (MCPX audio)
// Bus 0, Dev 6: ACI modem (AC97)
// Bus 0, Dev 9: IDE controller
// Bus 1, Dev 0: NV2A GPU

TEST_FUNC(HalReadWritePCISpace)
{
    TEST_BEGIN();

    typedef struct {
        const char* device_name;
        ULONG bus;
        ULONG slot;
        USHORT expected_vendor;
        USHORT expected_class; // upper 16 bits of class/rev register
    } pci_device_entry;

    pci_device_entry known_devices[] = {
        { "Host bridge",    0, 0, 0x10DE, 0x0600 },
        { "ISA bridge",     0, 1, 0x10DE, 0x0601 },
        { "USB OHCI #0",    0, 2, 0x10DE, 0x0C03 },
        // USB OHCI #1 is conditional - only present with internal USB hub
        { "NIC (nvnet)",    0, 4, 0x10DE, 0x0200 },
        { "APU",            0, 5, 0x10DE, 0x0401 },
        { "ACI (AC97)",     0, 6, 0x10DE, 0x0703 },
        { "IDE",            0, 9, 0x10DE, 0x0101 },
        { "NV2A GPU",       1, 0, 0x10DE, 0x0300 },
    };
    size_t num_devices = ARRAY_SIZE(known_devices);

    // --- Verify Vendor ID and Class Code for known Xbox PCI devices ---
    for (unsigned i = 0; i < num_devices; i++) {
        USHORT vendor_id = 0;
        HalReadWritePCISpace(known_devices[i].bus, known_devices[i].slot,
                             PCI_VENDOR_ID, &vendor_id, sizeof(vendor_id), FALSE);
        GEN_CHECK(vendor_id, known_devices[i].expected_vendor, known_devices[i].device_name);

        ULONG class_rev = 0;
        HalReadWritePCISpace(known_devices[i].bus, known_devices[i].slot,
                             PCI_CLASS_REV, &class_rev, sizeof(class_rev), FALSE);
        USHORT class_code = (USHORT)(class_rev >> 16);
        GEN_CHECK(class_code, known_devices[i].expected_class, known_devices[i].device_name);
    }

    // --- USB OHCI #1: only exists on systems with internal USB hub (daughterboard) ---
    if (XboxHardwareInfo.Flags & XBOX_HW_FLAG_INTERNAL_USB_HUB) {
        USHORT vendor_id = 0;
        HalReadWritePCISpace(0, 3, PCI_VENDOR_ID, &vendor_id, sizeof(vendor_id), FALSE);
        GEN_CHECK(vendor_id, (USHORT)0x10DE, "USB OHCI #1");

        ULONG class_rev = 0;
        HalReadWritePCISpace(0, 3, PCI_CLASS_REV, &class_rev, sizeof(class_rev), FALSE);
        USHORT class_code = (USHORT)(class_rev >> 16);
        GEN_CHECK(class_code, (USHORT)0x0C03, "USB OHCI #1");
    }

    // --- Consistency: reading same register twice gives same result ---
    USHORT v1 = 0, v2 = 0;
    HalReadWritePCISpace(0, 0, PCI_VENDOR_ID, &v1, sizeof(v1), FALSE);
    HalReadWritePCISpace(0, 0, PCI_VENDOR_ID, &v2, sizeof(v2), FALSE);
    GEN_CHECK(v1, v2, "consistent reads");

    // --- Non-existent device returns 0xFFFF ---
    USHORT nodev = 0;
    HalReadWritePCISpace(0, 31, PCI_VENDOR_ID, &nodev, sizeof(nodev), FALSE);
    GEN_CHECK(nodev, 0xFFFF, "non-existent device");

    // --- PCI write ---
    // SKIP: Writing to PCI config space can corrupt device configuration,
    // disable devices, or remap BARs causing system instability.
    // Only test reads.

    TEST_END();
}
