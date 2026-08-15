#include <xboxkrnl/xboxkrnl.h>
#include <nxdk/xbe.h>
#include <stdio.h>
#include <limits.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"
#include "assertions/common.h"

#define SECTION_STRING_TEST "Hello from custom section!"
#define SECTION_NAME_BLANK "secblank"
#define SECTION_NAME_TEST "sectest_"

#define SECTION_TESTS1_STR "section_tests1"
#define SECTION_TESTS2_STR "section_tests2"

// Create sections for testing
#pragma const_seg(SECTION_NAME_BLANK)
static const unsigned dummy_var = 1;
#pragma const_seg(SECTION_NAME_TEST)
static const char load_section_string[] = SECTION_STRING_TEST;
#pragma const_seg()

static const char preload_section_string[] = SECTION_STRING_TEST;

typedef NTSTATUS (NTAPI *sectionFuncAPI)(PXBE_SECTION_HEADER);

typedef struct {
    sectionFuncAPI nextFuncAPI;
    NTSTATUS expected_status;
    LONG expected_sectionCount;
    WORD expected_headCount;
    WORD expected_tailCount;
} section_test;

typedef struct {
    WORD* ptr_headCount;
    WORD* ptr_tailCount;
    WORD set_headCount;
    WORD set_tailCount;
} section_ref_test;

static BOOL preCheckList(PXBE_SECTION_HEADER* test_section) {
    const unsigned* do_not_use = &dummy_var; // Prevents the compiler from stripping the blank section
    (void)do_not_use; // Silence the unused variable warning
    BOOL TEST_GET_VAR = 1;

    // Find the blank and test sections to verify they were compiled correctly
    PXBE_SECTION_HEADER blank_section = nxXbeGetSectionByName(SECTION_NAME_BLANK);
    GEN_CHECK((blank_section != NULL), TRUE, "section_blank_exist");
    if (TEST_IS_FAILED) {
        return TEST_GET_VAR;
    }
    *test_section = nxXbeGetSectionByName(SECTION_NAME_TEST);
    GEN_CHECK((*test_section != NULL), TRUE, "section_test_exist");
    if (TEST_IS_FAILED) {
        return TEST_GET_VAR;
    }

    // Make sure that the section data is not loaded into memory initially
    BOOLEAN section_is_loaded = MmIsAddressValid((PVOID)load_section_string);
    if (section_is_loaded) {
        // If it is, we need to manually unload these sections in a specific order
        // to make the tests functional
        XeUnloadSection(blank_section);
        XeUnloadSection(*test_section);
        section_is_loaded = MmIsAddressValid((PVOID)load_section_string);
    }
    GEN_CHECK((!section_is_loaded), TRUE, "section_is_not_loaded");
    // Pre-check required to ensure section loading and unloading work properly
    GEN_CHECK((*test_section)->SectionReferenceCount, 0, "section_ref_count");
    GEN_CHECK((*(*test_section)->HeadReferenceCount), 0, "head_ref_count");
    GEN_CHECK((*(*test_section)->TailReferenceCount), 0, "tail_ref_count");

    return TEST_GET_VAR;
}

static signed getSectionPageStep(PXBE_SECTION_HEADER section) {
    // Check if the internal reference pointers are invalid
    if (section->HeadReferenceCount == NULL || section->TailReferenceCount == NULL) {
        return 0;
    }
    // Check if both reference pointers point to the same memory address
    // If they match, increment and decrement operations must be performed twice
    if (section->HeadReferenceCount == section->TailReferenceCount) {
        return 2;
    }
    // Otherwise, execute each increment and decrement operation once
    return 1;
}

TEST_FUNC(XeLoadSection)
{
    TEST_BEGIN();

    // Perform the pre-check sequence before initiating real functional tests
    PXBE_SECTION_HEADER test_section = NULL;
    TEST_GET_VAR = preCheckList(&test_section);
    if (TEST_IS_FAILED) {
        print("An error is found from either nxdk's toolchain or the kernel's initialization.");
        TEST_END();
    }

    // Obtain the programmable page step under certain conditions which may be subject to
    // change depending on nxdk toolchain updates over time
    signed page_step = getSectionPageStep(test_section);
    if (page_step == 0) {
        print("An error is found from nxdk's tool chain, please fix this issue first.");
        TEST_END();
    }

    // Preserve the original reference pointers to restore them later
    WORD* pOrgHeadRefCount = test_section->HeadReferenceCount;
    WORD* pOrgTailRefCount = test_section->TailReferenceCount;

    // NOTE: Invalid sections cannot be tested here; doing so may disrupt the kernel's internal lock mechanism.

    // Test #1: Ensure that the section data is not loaded into memory initially
    BOOLEAN section_is_loaded = MmIsAddressValid((PVOID)load_section_string);
    GEN_CHECK((!section_is_loaded), TRUE, "section_is_not_loaded");

    // Test #2: Load the section to perform the initial verification
    NTSTATUS status = XeLoadSection(test_section);
    assert_NTSTATUS(status, STATUS_SUCCESS, TEST_GET_API_NAME);
    if (TEST_IS_FAILED) {
        TEST_END();
    }

    // Test #3: Verify that all reference counters incremented successfully
    GEN_CHECK(test_section->SectionReferenceCount, 1, "section_ref_count");
    GEN_CHECK((*test_section->HeadReferenceCount), page_step, "head_ref_count");
    GEN_CHECK((*test_section->TailReferenceCount), page_step, "tail_ref_count");
    // Ensure that the section data is now mapped and visible in memory
    section_is_loaded = MmIsAddressValid((PVOID)load_section_string);
    GEN_CHECK(section_is_loaded, TRUE, "section_is_loaded");
    if (section_is_loaded) {
        // Verify that both strings do match
        GEN_CHECK_ARRAY(load_section_string, preload_section_string, ARRAY_SIZE(preload_section_string), "section_test_string");
    }
    (void)XeUnloadSection(test_section);
    if (TEST_IS_FAILED) {
        TEST_END();
    }

    // Test #4: Perform progressive loop execution verification
    ULONG section_tests1[] = {
        // Single load test (already validated above)
        // Double load test
        2,
        // Low upper-bound test (avoids taking hours to load and unload)
        USHRT_MAX + 1, // NOTE: LONG_MAX will take at least 22 hours to complete
    };
    for (unsigned i = 0; i < ARRAY_SIZE(section_tests1); i++) {
        ULONG ii = 0;
        // Test #4a: Progressively load the section to evaluate reference increment
        for (; ii < section_tests1[i];) {
            ii++;
            NTSTATUS status = XeLoadSection(test_section);
            GEN_CHECK_ARRAY_MEMBER_ARRAY_DIRECT(status, STATUS_SUCCESS, SECTION_TESTS1_STR, i, "return_status", ii);
            GEN_CHECK_ARRAY_MEMBER_ARRAY_DIRECT(test_section->SectionReferenceCount, ii, SECTION_TESTS1_STR, i, "section_ref_count", ii);
            GEN_CHECK_ARRAY_MEMBER_ARRAY_DIRECT((*test_section->HeadReferenceCount), page_step, SECTION_TESTS1_STR, i, "head_ref_count", ii);
            GEN_CHECK_ARRAY_MEMBER_ARRAY_DIRECT((*test_section->TailReferenceCount), page_step, SECTION_TESTS1_STR, i, "tail_ref_count", ii);
            section_is_loaded = MmIsAddressValid((PVOID)load_section_string);
            GEN_CHECK_ARRAY_MEMBER_ARRAY_DIRECT(section_is_loaded, TRUE, SECTION_TESTS1_STR, i, "section_is_loaded", ii);
            if (TEST_IS_FAILED) {
                break;
            }
        }
        // Unwind reference counts to reset the section for the next test iteration
        for (; ii > 0; ii--) {
            (void)XeUnloadSection(test_section);
        }
        if (TEST_IS_FAILED) {
            TEST_END();
        }
    }

    // Test #5: Verify modified section head and tail reference pointer behavior
    WORD tmpHeadRefCount;
    WORD tmpTailRefCount;
    section_ref_test section_tests2[] = {
        // Head and tail reference pointer test configurations
        { .ptr_headCount = &tmpHeadRefCount, .ptr_tailCount = &tmpTailRefCount, .set_headCount = 5, .set_tailCount = 3 },
        { .ptr_headCount = &tmpHeadRefCount, .ptr_tailCount = &tmpTailRefCount, .set_headCount = 3, .set_tailCount = 5 },
        { .ptr_headCount = &tmpHeadRefCount, .ptr_tailCount = &tmpHeadRefCount, .set_headCount = 5, .set_tailCount = 5 },
    };
    for (unsigned i = 0; i < ARRAY_SIZE(section_tests2); i++) {
        // Set the temporary pointer and reference counts to run the test
        test_section->HeadReferenceCount = section_tests2[i].ptr_headCount;
        test_section->TailReferenceCount = section_tests2[i].ptr_tailCount;
        (*test_section->HeadReferenceCount) = section_tests2[i].set_headCount;
        (*test_section->TailReferenceCount) = section_tests2[i].set_tailCount;
        signed init_step = getSectionPageStep(test_section);

        NTSTATUS status = XeLoadSection(test_section);
        GEN_CHECK_ARRAY_MEMBER_DIRECT(status, STATUS_SUCCESS, SECTION_TESTS2_STR, i, "return_status");
        GEN_CHECK_ARRAY_MEMBER_DIRECT(test_section->SectionReferenceCount, 1, SECTION_TESTS2_STR, i, "section_ref_count");
        GEN_CHECK_ARRAY_MEMBER_DIRECT((*test_section->HeadReferenceCount), (section_tests2[i].set_headCount + init_step), SECTION_TESTS2_STR, i, "head_ref_count");
        GEN_CHECK_ARRAY_MEMBER_DIRECT((*test_section->TailReferenceCount), (section_tests2[i].set_tailCount + init_step), SECTION_TESTS2_STR, i, "tail_ref_count");
        section_is_loaded = MmIsAddressValid((PVOID)load_section_string);
        GEN_CHECK_ARRAY_MEMBER_DIRECT(section_is_loaded, TRUE, SECTION_TESTS2_STR, i, "section_is_loaded");

        // Safely unload the section to reset state for the next test iteration
        (*test_section->HeadReferenceCount) = init_step;
        (*test_section->TailReferenceCount) = init_step;
        (void)XeUnloadSection(test_section);
        // Ensure the target section is no longer loaded in memory
        section_is_loaded = MmIsAddressValid((PVOID)load_section_string);
        GEN_CHECK_ARRAY_MEMBER_DIRECT(!section_is_loaded, TRUE, SECTION_TESTS2_STR, i, "section_is_not_loaded");
        if (TEST_IS_FAILED) {
            break;
        }
    }

    // Restore the original reference pointers
    test_section->HeadReferenceCount = pOrgHeadRefCount;
    test_section->TailReferenceCount = pOrgTailRefCount;

    // Test #6: Verify SectionReferenceCount can increment to the LONG_MAX value without issue
    (void)XeLoadSection(test_section);
    test_section->SectionReferenceCount = LONG_MAX - 1;
    status = XeLoadSection(test_section);
    assert_NTSTATUS(status, STATUS_SUCCESS, TEST_GET_API_NAME);
    GEN_CHECK(test_section->SectionReferenceCount, LONG_MAX, "section_ref_count");
    section_is_loaded = MmIsAddressValid((PVOID)load_section_string);
    GEN_CHECK(section_is_loaded, TRUE, "section_is_loaded");
    test_section->SectionReferenceCount = 1;
    (void)XeUnloadSection(test_section);

    TEST_END();
}

TEST_FUNC(XeUnloadSection)
{
    TEST_BEGIN();

    NTSTATUS status;
    BOOLEAN section_is_loaded;

    // Perform the pre-check sequence before initiating real functional tests
    PXBE_SECTION_HEADER test_section = NULL;
    TEST_GET_VAR = preCheckList(&test_section);
    if (TEST_IS_FAILED) {
        TEST_END();
    }

    // Obtain the programmable page step under certain conditions which may be subject to
    // change depending on nxdk toolchain updates over time
    signed page_step = getSectionPageStep(test_section);
    if (page_step == 0) {
        TEST_SKIP_END("An error is found from nxdk's tool chain, please fix this issue first.");
    }

    // Test #1: Verify that unloading an unmapped section fails as expected
    status = XeUnloadSection(test_section);
    assert_NTSTATUS(status, STATUS_INVALID_PARAMETER, TEST_GET_API_NAME);
    if (TEST_IS_FAILED) {
        TEST_END();
    }
    GEN_CHECK(test_section->SectionReferenceCount, 0, "section_ref_count");
    GEN_CHECK((*test_section->HeadReferenceCount), 0, "head_ref_count");
    GEN_CHECK((*test_section->TailReferenceCount), 0, "tail_ref_count");
    if (TEST_IS_FAILED) {
        TEST_END();
    }

    // TODO: NOTE: We could make a mock section here if needed

    // Test #2: Perform progressive loop execution verification
    char formatted_str[128];
    ULONG section_tests1[] = {
        // Single unload test
        1,
        // Double unload test
        2,
        // Low upper-bound test (avoids taking hours to load and unload)
        USHRT_MAX + 1, // NOTE: LONG_MAX will take at least 22 hours to complete
    };
    for (unsigned i = 0; i < ARRAY_SIZE(section_tests1); i++) {
        ULONG ii = 0;
        // Prime reference counts up to establish the baseline for unload verification
        for (; ii < section_tests1[i]; ii++) {
            (void)XeLoadSection(test_section);
        }
        // Test #2a: Progressively unload the section to evaluate reference decrement
        for (; ii > 0;) {
            ii--;
            status = XeUnloadSection(test_section);
            GEN_CHECK_ARRAY_MEMBER_ARRAY_DIRECT(status, STATUS_SUCCESS, SECTION_TESTS1_STR, i, "return_status", ii);
            GEN_CHECK_ARRAY_MEMBER_ARRAY_DIRECT(test_section->SectionReferenceCount, ii, SECTION_TESTS1_STR, i, "section_ref_count", ii);
            GEN_CHECK_ARRAY_MEMBER_ARRAY_DIRECT((*test_section->HeadReferenceCount), (ii ? page_step : 0), SECTION_TESTS1_STR, i, "head_ref_count", ii);
            GEN_CHECK_ARRAY_MEMBER_ARRAY_DIRECT((*test_section->TailReferenceCount), (ii ? page_step : 0), SECTION_TESTS1_STR, i, "tail_ref_count", ii);
            section_is_loaded = MmIsAddressValid((PVOID)load_section_string);
            GEN_CHECK_ARRAY_MEMBER_ARRAY_DIRECT(section_is_loaded, (ii != 0), SECTION_TESTS1_STR, i, "section_is_loaded", ii);
            if (TEST_IS_FAILED) {
                break;
            }
        }
        // Test #2b: Verify the extra unload call will return an error code
        status = XeUnloadSection(test_section);
        GEN_CHECK_ARRAY_MEMBER_DIRECT(status, STATUS_INVALID_PARAMETER, SECTION_TESTS1_STR, i, "return_status");
        if (TEST_IS_FAILED) {
            TEST_END();
        }
    }

    // Preserve the original reference pointers and value for later restoration
    (void)XeLoadSection(test_section);
    WORD* pOrgHeadRefCount = test_section->HeadReferenceCount;
    WORD* pOrgTailRefCount = test_section->TailReferenceCount;

    // Test #3: Verify modified section head and tail reference pointer behavior
    WORD tmpHeadRefCount;
    WORD tmpTailRefCount;
    section_ref_test section_tests2[] = {
        // Head and tail reference pointer test configurations
        { .ptr_headCount = &tmpHeadRefCount, .ptr_tailCount = &tmpTailRefCount, .set_headCount = 5, .set_tailCount = 3 },
        { .ptr_headCount = &tmpHeadRefCount, .ptr_tailCount = &tmpTailRefCount, .set_headCount = 3, .set_tailCount = 5 },
        { .ptr_headCount = &tmpHeadRefCount, .ptr_tailCount = &tmpHeadRefCount, .set_headCount = 5, .set_tailCount = 5 },
    };
    for (unsigned i = 0; i < ARRAY_SIZE(section_tests2); i++) {
        // Set the temporary pointer and reference counts to run the test
        test_section->HeadReferenceCount = section_tests2[i].ptr_headCount;
        test_section->TailReferenceCount = section_tests2[i].ptr_tailCount;
        (*test_section->HeadReferenceCount) = section_tests2[i].set_headCount;
        (*test_section->TailReferenceCount) = section_tests2[i].set_tailCount;
        signed init_step = getSectionPageStep(test_section);

        status = XeUnloadSection(test_section);
        GEN_CHECK_ARRAY_MEMBER_DIRECT(status, STATUS_SUCCESS, SECTION_TESTS2_STR, i, "return_status");
        GEN_CHECK_ARRAY_MEMBER_DIRECT(test_section->SectionReferenceCount, 0, SECTION_TESTS2_STR, i, "section_ref_count");
        GEN_CHECK_ARRAY_MEMBER_DIRECT((*test_section->HeadReferenceCount), (section_tests2[i].set_headCount - init_step), SECTION_TESTS2_STR, i, "head_ref_count");
        GEN_CHECK_ARRAY_MEMBER_DIRECT((*test_section->TailReferenceCount), (section_tests2[i].set_tailCount - init_step), SECTION_TESTS2_STR, i, "tail_ref_count");
        section_is_loaded = MmIsAddressValid((PVOID)load_section_string);
        GEN_CHECK_ARRAY_MEMBER_DIRECT(section_is_loaded, TRUE, SECTION_TESTS2_STR, i, "section_is_loaded");

        // Prime the section again to prepare for the next loop test
        (void)XeLoadSection(test_section);

        if (TEST_IS_FAILED) {
            break;
        }
    }

    // Restore the original reference pointers
    test_section->HeadReferenceCount = pOrgHeadRefCount;
    test_section->TailReferenceCount = pOrgTailRefCount;

    // Test #4: Make sure the section data is properly unloaded after Test #3 execution
    status = XeUnloadSection(test_section);
    assert_NTSTATUS(status, STATUS_SUCCESS, TEST_GET_API_NAME);
    GEN_CHECK(test_section->SectionReferenceCount, 0, "section_ref_count");
    GEN_CHECK((*test_section->HeadReferenceCount), 0, "head_ref_count");
    GEN_CHECK((*test_section->TailReferenceCount), 0, "tail_ref_count");
    section_is_loaded = MmIsAddressValid((PVOID)load_section_string);
    GEN_CHECK(!section_is_loaded, TRUE, "section_is_not_loaded");
    if (TEST_IS_FAILED) {
        TEST_END();
    }

    // Test #5a: Verify SectionReferenceCount can decrement from the LONG_MAX value without issue
    (void)XeLoadSection(test_section);
    test_section->SectionReferenceCount = LONG_MAX;
    status = XeUnloadSection(test_section);
    assert_NTSTATUS(status, STATUS_SUCCESS, TEST_GET_API_NAME);
    GEN_CHECK(test_section->SectionReferenceCount, LONG_MAX - 1, "section_ref_count");
    section_is_loaded = MmIsAddressValid((PVOID)load_section_string);
    GEN_CHECK(section_is_loaded, TRUE, "section_is_loaded");

    // Test #5b: Ensure proper unmapping of section data from the previous test
    test_section->SectionReferenceCount = 1;
    status = XeUnloadSection(test_section);
    assert_NTSTATUS(status, STATUS_SUCCESS, TEST_GET_API_NAME);
    GEN_CHECK(test_section->SectionReferenceCount, 0, "section_ref_count");
    section_is_loaded = MmIsAddressValid((PVOID)load_section_string);
    GEN_CHECK(!section_is_loaded, TRUE, "section_is_not_loaded");

    TEST_END();
}
