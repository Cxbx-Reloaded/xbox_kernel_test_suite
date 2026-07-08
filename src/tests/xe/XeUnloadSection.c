#include <xboxkrnl/xboxkrnl.h>
#include <nxdk/xbe.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(XeUnloadSection)
{
    TEST_BEGIN();

    // XeUnloadSection decrements the reference count of a loaded section.
    // When the count reaches zero, the section is paged out.
    // We test with an already-loaded preloaded section.

    PXBE_SECTION_HEADER section = nxXbeGetSectionByName(".text");
    if (section == NULL) {
        section = nxXbeGetSectionByName(".rdata");
    }

    BOOL found_section = (section != NULL);
    GEN_CHECK(found_section, TRUE, "found a section to test");

    if (section != NULL) {
        LONG initial_refcount = section->SectionReferenceCount;
        BOOL refcount_positive = (initial_refcount > 0);
        GEN_CHECK(refcount_positive, TRUE, "initial refcount > 0");

        // First, bump the reference count so we can safely decrement
        NTSTATUS status = XeLoadSection(section);
        GEN_CHECK(status, STATUS_SUCCESS, "prep: XeLoadSection SUCCESS");

        LONG after_load = section->SectionReferenceCount;
        GEN_CHECK(after_load, initial_refcount + 1, "prep: refcount bumped");

        // Now test XeUnloadSection
        status = XeUnloadSection(section);
        GEN_CHECK(status, STATUS_SUCCESS, "XeUnloadSection returns SUCCESS");

        LONG after_unload = section->SectionReferenceCount;
        GEN_CHECK(after_unload, initial_refcount, "refcount decremented");

        // Multiple load/unload cycles
        for (ULONG i = 0; i < 3; i++) {
            XeLoadSection(section);
        }
        LONG after_3loads = section->SectionReferenceCount;
        GEN_CHECK(after_3loads, initial_refcount + 3, "3 loads bumped by 3");

        for (ULONG i = 0; i < 3; i++) {
            XeUnloadSection(section);
        }
        LONG after_3unloads = section->SectionReferenceCount;
        GEN_CHECK(after_3unloads, initial_refcount, "3 unloads restored");
    }

    // SKIP: Unloading a section to refcount 0 when it's the .text section
    //       would unmap the code we're executing - instant crash.
    // SKIP: Unloading a NULL section - would crash/bugcheck.
    // SKIP: Testing actual page-out behavior requires a demand-loaded section
    //       (non-preloaded) which needs XBE build system support to create.

    TEST_END();
}
