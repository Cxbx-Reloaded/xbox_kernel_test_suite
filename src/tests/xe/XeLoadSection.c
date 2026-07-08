#include <xboxkrnl/xboxkrnl.h>
#include <nxdk/xbe.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(XeLoadSection)
{
    TEST_BEGIN();

    // XeLoadSection loads an XBE section or increments its reference count.
    // We test with an already-loaded section from the running XBE.
    // The section must exist in the XBE's section header table.

    // Find the first section in the XBE (typically ".text" or similar)
    PXBE_SECTION_HEADER section = nxXbeGetSectionByName(".text");
    if (section == NULL) {
        // Fallback: try .rdata
        section = nxXbeGetSectionByName(".rdata");
    }

    BOOL found_section = (section != NULL);
    GEN_CHECK(found_section, TRUE, "found a section to test");

    if (section != NULL) {
        // Record the initial reference count
        LONG initial_refcount = section->SectionReferenceCount;

        // The section is already loaded (it's part of the running XBE),
        // so SectionReferenceCount should be > 0 for preloaded sections.
        BOOL refcount_positive = (initial_refcount > 0);
        GEN_CHECK(refcount_positive, TRUE, "initial refcount > 0");

        // XeLoadSection should increment the reference count and return STATUS_SUCCESS
        NTSTATUS status = XeLoadSection(section);
        GEN_CHECK(status, STATUS_SUCCESS, "XeLoadSection returns SUCCESS");

        LONG after_load = section->SectionReferenceCount;
        GEN_CHECK(after_load, initial_refcount + 1, "refcount incremented");

        // Call again - should increment again
        status = XeLoadSection(section);
        GEN_CHECK(status, STATUS_SUCCESS, "XeLoadSection second call SUCCESS");

        LONG after_load2 = section->SectionReferenceCount;
        GEN_CHECK(after_load2, initial_refcount + 2, "refcount incremented twice");

        // Balance by unloading twice to restore original state
        XeUnloadSection(section);
        XeUnloadSection(section);

        LONG restored = section->SectionReferenceCount;
        GEN_CHECK(restored, initial_refcount, "refcount restored after unloads");
    }

    // SKIP: Testing with a NULL section pointer - would likely crash/bugcheck.
    // SKIP: Testing with a fabricated section header - undefined behavior,
    //       the kernel would attempt to page in from the XBE file at invalid offsets.
    // SKIP: Testing with a truly unloaded demand-loaded section requires the XBE
    //       to contain a non-preloaded section, which needs build system support
    //       (cxbe flags to mark a section as demand-loaded).

    TEST_END();
}
