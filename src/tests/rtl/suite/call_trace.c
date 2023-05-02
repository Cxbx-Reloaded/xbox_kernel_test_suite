#include <xboxkrnl/xboxkrnl.h>
#include <stdlib.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

void test_RtlCaptureStackBackTrace(){
    const char* func_num = "0x010A";
    const char* func_name = "RtlCaptureStackBackTrace";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    ULONG frame_size = 10;
    PULONG frames_expected = (PULONG)malloc(frame_size * sizeof(ULONG));
    PULONG frames_test = (PULONG)malloc(frame_size * sizeof(ULONG));

    // First, we need to collect frames_expected to perform later tests.
    frame_size = RtlWalkFrameChain((PVOID*)frames_expected, frame_size, 0);
    if (!frame_size) {
        // If we are not getting any return size, force fail.
        GEN_CHECK(frame_size, 1, "frame_size");
    }
    else {
        // Check all frames capture without any skip.
        ULONG frames_skip = 1; // We want to skip at least one due to the fact it point to first call within kernel.
        ULONG frames_capture = frame_size - 1;

        ULONG frames_hash_expected = 0, frames_hash_test = 0;
        USHORT frames_count = RtlCaptureStackBackTrace(frames_skip, frames_capture, (PVOID*)frames_test, &frames_hash_test);

        GEN_CHECK(frames_count, frames_capture, "frames_count1");

        PULONG frames_expected_offset = frames_expected + frames_skip;
        GEN_CHECK_ARRAY(frames_test, frames_expected_offset, frames_count, "frames1");

        for (unsigned i = frames_skip; i < frames_skip + frames_count; i++) {
            frames_hash_expected += frames_expected[i];
        }
        GEN_CHECK(frames_hash_test, frames_hash_expected, "frames_hash1");

        // Choose specific portion of the frames we want to check against.
        frames_skip = frame_size / 2;
        frames_capture = frame_size / 2;

        frames_hash_expected = frames_hash_test = 0;
        frames_count = RtlCaptureStackBackTrace(frames_skip, frames_capture, (PVOID*)frames_test, &frames_hash_test);

        GEN_CHECK(frames_count, frames_capture, "frames_count2");

        frames_expected_offset = frames_expected + frames_skip;
        GEN_CHECK_ARRAY(frames_test, frames_expected_offset, frames_count, "frames2");

        for (unsigned i = frames_skip; i < frames_skip + frames_count; i++) {
            frames_hash_expected += frames_expected[i];
        }
        GEN_CHECK(frames_hash_test, frames_hash_expected, "frames_hash2");

        // Choose specific portion of the frames we want to check against.
        frames_skip = frame_size / 2;
        frames_capture = frames_skip / 2;

        frames_hash_expected = frames_hash_test = 0;
        frames_count = RtlCaptureStackBackTrace(frames_skip, frames_capture, (PVOID*)frames_test, &frames_hash_test);

        GEN_CHECK(frames_count, frames_capture, "frames_count3");

        frames_expected_offset = frames_expected + frames_skip;
        GEN_CHECK_ARRAY(frames_test, frames_expected_offset, frames_count, "frames3");

        for (unsigned i = frames_skip; i < frames_skip + frames_count; i++) {
            frames_hash_expected += frames_expected[i];
        }
        GEN_CHECK(frames_hash_test, frames_hash_expected, "frames_hash3");
    }

    free(frames_expected);
    free(frames_test);

    print_test_footer(func_num, func_name, test_passed);
}

void stub_RtlGetCallersAddress(PVOID* CallerAddress_test, PVOID* CallersCaller_test, PULONG CallerAddress_expected){
    *CallerAddress_expected = (ULONG)__builtin_return_address(0);
    RtlGetCallersAddress(CallerAddress_test, CallersCaller_test);
}

void test_RtlGetCallersAddress(){
    const char* func_num = "0x0120";
    const char* func_name = "RtlGetCallersAddress";
    BOOL test_passed = 1;

    print_test_header(func_num, func_name);

    ULONG CallerAddress_test, CallersCaller_test, CallerAddress_expected;
    ULONG CallersCaller_expected = (ULONG)__builtin_return_address(0);

    stub_RtlGetCallersAddress((PVOID*)&CallerAddress_test, (PVOID*)&CallersCaller_test, &CallerAddress_expected);

    GEN_CHECK(CallerAddress_test, CallerAddress_expected, "CallerAddress");

    GEN_CHECK(CallersCaller_test, CallersCaller_expected, "CallersCaller");

    print_test_footer(func_num, func_name, test_passed);
}

// stack calls to see how RtlWalkFrameChain work internally.
static ULONG stub_RtlWalkFrameChain4of4(PVOID* Callers, ULONG Count) {
    return RtlWalkFrameChain(Callers, Count, 0/*Flags parameter is unused*/);
}
static ULONG stub_RtlWalkFrameChain3of4(PVOID* Callers, ULONG Count) {
    return stub_RtlWalkFrameChain4of4(Callers, Count);
}
static ULONG stub_RtlWalkFrameChain2of4(PVOID* Callers, ULONG Count) {
    return stub_RtlWalkFrameChain3of4(Callers, Count);
}
static ULONG stub_RtlWalkFrameChain1of4(PVOID* Callers, ULONG Count) {
    return stub_RtlWalkFrameChain2of4(Callers, Count);
}

void test_RtlWalkFrameChain(){
    const char* func_num = "0x013F";
    const char* func_name = "RtlWalkFrameChain";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    ULONG count = 20;
    ULONG callers[count];
    memset(callers, 0, sizeof(callers));

    ULONG total = stub_RtlWalkFrameChain1of4((PVOID*)callers, count);

    // Last time checked on hardware, there's total of 8 calls.
    GEN_CHECK(total, 8, "total");

    // We are checking for child calls in this function for verification.
    GEN_CHECK_RANGE(callers[0], (ULONG)(PVOID)stub_RtlWalkFrameChain4of4, 0x50, "callers[0]");
    GEN_CHECK_RANGE(callers[1], (ULONG)(PVOID)stub_RtlWalkFrameChain3of4, 0x30, "callers[1]");
    GEN_CHECK_RANGE(callers[2], (ULONG)(PVOID)stub_RtlWalkFrameChain2of4, 0x30, "callers[2]");
    GEN_CHECK_RANGE(callers[3], (ULONG)(PVOID)stub_RtlWalkFrameChain1of4, 0x30, "callers[3]");

    // We don't check test_RtlWalkFrameChain as it can change at any time.

    // Output any remaining parent calls occur from the stack.
    // We do not make any parent calls address verification as they can change at any time.
    for (ULONG i = 4; i < total; i++) {
        print("  INFO: callers[%u] = 0x%08x", i, callers[i]);
    }

    print_test_footer(func_num, func_name, test_passed);
}
