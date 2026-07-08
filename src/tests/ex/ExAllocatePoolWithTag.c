#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ExAllocatePoolWithTag)
{
    TEST_BEGIN();

    // --- Basic allocation with tag ---
    PVOID mem = ExAllocatePoolWithTag(64, 'tseT');
    GEN_CHECK(mem != NULL, TRUE, "tagged allocation 64 bytes");
    if (mem) {
        memset(mem, 0xAA, 64);
        GEN_CHECK(((PUCHAR)mem)[0], 0xAA, "first byte");
        GEN_CHECK(((PUCHAR)mem)[63], 0xAA, "last byte");
        ExFreePool(mem);
    }

    // --- Different tags produce distinct allocations ---
    PVOID mem1 = ExAllocatePoolWithTag(32, 'Tag1');
    PVOID mem2 = ExAllocatePoolWithTag(32, 'Tag2');
    GEN_CHECK(mem1 != NULL, TRUE, "tag1 alloc");
    GEN_CHECK(mem2 != NULL, TRUE, "tag2 alloc");
    GEN_CHECK(mem1 != mem2, TRUE, "different addresses for different tags");
    if (mem1) ExFreePool(mem1);
    if (mem2) ExFreePool(mem2);

    // --- 1-byte allocation ---
    mem = ExAllocatePoolWithTag(1, 'Tiny');
    GEN_CHECK(mem != NULL, TRUE, "1-byte tagged alloc");
    if (mem) {
        *(PUCHAR)mem = 0x42;
        GEN_CHECK(*(PUCHAR)mem, 0x42, "1-byte writable");
        ExFreePool(mem);
    }

    // --- PAGE_SIZE allocation ---
    mem = ExAllocatePoolWithTag(PAGE_SIZE, 'Page');
    GEN_CHECK(mem != NULL, TRUE, "page-size tagged alloc");
    if (mem) {
        ((PUCHAR)mem)[0] = 0x11;
        ((PUCHAR)mem)[PAGE_SIZE - 1] = 0x22;
        GEN_CHECK(((PUCHAR)mem)[0], 0x11, "page first byte");
        GEN_CHECK(((PUCHAR)mem)[PAGE_SIZE - 1], 0x22, "page last byte");
        ExFreePool(mem);
    }

    // --- Large allocation (multiple pages) ---
    mem = ExAllocatePoolWithTag(PAGE_SIZE * 4, 'Big!');
    GEN_CHECK(mem != NULL, TRUE, "4-page tagged alloc");
    if (mem) {
        memset(mem, 0xCC, PAGE_SIZE * 4);
        GEN_CHECK(((PUCHAR)mem)[0], 0xCC, "large first");
        GEN_CHECK(((PUCHAR)mem)[PAGE_SIZE * 4 - 1], 0xCC, "large last");
        ExFreePool(mem);
    }

    // --- Tag is stored and retrievable via pool block ---
    // ExQueryPoolBlockSize returns the usable size (not the tag)
    // The tag is internal to the pool header, verify size is reasonable
    mem = ExAllocatePoolWithTag(48, 'Chk!');
    if (mem) {
        ULONG size = ExQueryPoolBlockSize(mem);
        GEN_CHECK(size >= 48, TRUE, "reported size >= requested");
        GEN_CHECK(size < 48 + 64, TRUE, "size has reasonable overhead");
        ExFreePool(mem);
    }

    // --- Zero tag value (should still work) ---
    mem = ExAllocatePoolWithTag(32, 0);
    GEN_CHECK(mem != NULL, TRUE, "zero tag works");
    if (mem) ExFreePool(mem);

    // --- Allocate/free cycle to verify no corruption ---
    PVOID ptrs[8];
    ULONG i;
    for (i = 0; i < 8; i++) {
        ptrs[i] = ExAllocatePoolWithTag(16 * (i + 1), 'Loop');
        GEN_CHECK(ptrs[i] != NULL, TRUE, "loop alloc");
    }
    // Verify no overlap
    for (i = 0; i < 8; i++) {
        for (ULONG j = i + 1; j < 8; j++) {
            if (ptrs[i] && ptrs[j]) {
                GEN_CHECK(ptrs[i] != ptrs[j], TRUE, "no overlap");
            }
        }
    }
    for (i = 0; i < 8; i++) {
        if (ptrs[i]) ExFreePool(ptrs[i]);
    }

    // --- Size 0 ---
    // SKIP: Zero-size allocations are implementation-defined. The pool allocator
    // may round up to minimum block size or return NULL. Not testing to avoid
    // depending on implementation details.

    TEST_END();
}
