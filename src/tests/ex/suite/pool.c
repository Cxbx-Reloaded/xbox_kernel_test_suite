#include <xboxkrnl/xboxkrnl.h>
#include <stdlib.h>
#include <stdio.h>

#include "global.h"
#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

#define TEST_TAG        'TSTP'
#define MAX_BLOCKS      512
#define MAX_EXHAUST     2048      /* cap on 4 KB blocks (8 MB) to protect the console */
#define PAGE_SLACK      4         /* tolerated page-accounting noise */
#define CYCLES          20
#define STRESS_BLOCKS   64


typedef BOOL(*test_fn)(const char*, BOOL);

static BOOLEAN HasTestedPoolAlloc = FALSE;


static ULONG GetPoolPages(PULONG NumOfPoolPages)
{
    MM_STATISTICS stats;
    NTSTATUS status;

    stats.Length = sizeof(stats);
    status = MmQueryStatistics(&stats);
    if (!NT_SUCCESS(status)) {
        return FALSE;
    }
    
    *NumOfPoolPages = stats.PoolPagesCommitted;
    return TRUE;
}

static LONG PagesDelta(ULONG after, ULONG before)
{
    return (LONG)after - (LONG)before;
}

static ULONG RoundUpPage(ULONG n)
{
    return (n + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
}

/* Checks the block size reported for a request. Overhead above the threshold is a warning. */
static BOOL CheckOverhead(ULONG req, ULONG blk)
{
    BOOL TEST_GET_VAR = 1;
    ULONG limit;

    if (blk < req) {
        TEST_FAILED();
        print("  ERROR: block size %lu is smaller than request %lu", blk, req);
        return TEST_GET_VAR;
    }
    if (req <= PAGE_SIZE) {
        limit = req + req / 2 + 32;      /* small-block overhead tolerance */
    }
    else {
        limit = RoundUpPage(req);        /* large blocks should be page-granular */
    }

    if (blk > limit) {
        print("  WARN: request %lu got block %lu (overhead %lu, limit %lu)\n",
                  req, blk, blk - req, limit);
    }
    
    return TEST_GET_VAR;
}

/* 1. Basic correctness across size classes */
static BOOL TestBasicSizes(const char* test_name, BOOL has_tag)
{
    ASSERT_HEADER(test_name);
    
    static const ULONG sizes[] = {
        1, 7, 8, 16, 31, 32, 33, 64, 100, 128, 255, 256, 500, 512, 1000,
        1024, 2047, 2048, 4095, 4096, 4097, 8192, 16384, 65536
    };
    ULONG i, j, n = ARRAY_SIZE(sizes);

    for (i = 0; i < n; i++) {
        ULONG req = sizes[i], blk, pattern;
        UCHAR *p;

        p = has_tag ? (UCHAR *)ExAllocatePoolWithTag(req, TEST_TAG) : (UCHAR *)ExAllocatePool(req);
        GEN_CHECK(p != NULL, TRUE, "valid allocation must succeed");
        if (p == NULL) {
            print("  ERROR: alloc of %lu bytes returned NULL", req);
            continue;
        }
        GEN_CHECK(((ULONG)p & 7) == 0, TRUE, "allocation pointer aligned to 8 bytes");
        if (((ULONG)p & 7) != 0) {
            print("  ERROR: alloc %lu, pointer 0x%x is not 8-byte aligned", req, (ULONG)p);
        }

        blk = ExQueryPoolBlockSize(p);
        GEN_CHECK((blk != 0), TRUE, "allocated block size must not be zero");
        if (blk == 0) {
            print("  ERROR: alloc %lu, ExQueryPoolBlockSize returned 0", req);
        }
        else {
            TEST_GET_VAR &= CheckOverhead(req, blk);

            /* The whole reported block must be writable and must keep its contents */
            pattern = i;
            for (j = 0; j < blk; j++) {
                p[j] = (UCHAR)(j * 7 + pattern);
            }
            for (j = 0; j < blk; j++) {
                if (p[j] != (UCHAR)(j * 7 + pattern)) {
                    TEST_FAILED();
                    print("  ERROR: alloc %lu, data corrupted at offset %lu", req, j);
                    break;
                }
            }
        }
        ExFreePool(p);
    }

    /* Zero-byte request: behaviour is implementation-defined, so only report it */
    {
        UCHAR *z = has_tag ? (UCHAR *)ExAllocatePoolWithTag(0, TEST_TAG) : (UCHAR *)ExAllocatePool(0);
        print("  INFO: 0-byte request returned 0x%x", (ULONG)z);
        if (z != NULL) {
            print("  INFO: 0-byte block size = %lu", ExQueryPoolBlockSize(z));
            ExFreePool(z);
        }
    }
    
    ASSERT_FOOTER(test_name);
}

/* 2. Absurd sizes must fail cleanly and must not change pool usage */
static BOOL TestInvalidSizes(const char* test_name, BOOL has_tag)
{
    ASSERT_HEADER(test_name);
    
    static const ULONG bad[] = { 0x7FFFFFF0UL, 0x80000000UL, 0xFFFFFFFFUL };
    ULONG i, before, after, mm_ret;

    mm_ret = GetPoolPages(&before);
    GEN_CHECK(mm_ret == TRUE, TRUE, "MmQueryStatistics failed");
    if (mm_ret == FALSE) {
        ASSERT_FOOTER(test_name);
    }
    for (i = 0; i < 3; i++) {
        void *p = has_tag ? ExAllocatePoolWithTag(bad[i], TEST_TAG) : ExAllocatePool(bad[i]);
        GEN_CHECK(p == NULL, TRUE, "invalid allocation must fail");
        if (p != NULL) {
            print("  ERROR: huge request 0x%x unexpectedly succeeded", bad[i]);
            ExFreePool(p);
        }
    }
    mm_ret = GetPoolPages(&after);
    GEN_CHECK(mm_ret == TRUE, TRUE, "MmQueryStatistics failed");
    if (mm_ret == FALSE) {
        ASSERT_FOOTER(test_name);
    }
    GEN_CHECK(PagesDelta(after, before) <= PAGE_SLACK, TRUE, "failed huge allocations must leave the pool usage untouched");
    if (PagesDelta(after, before) > PAGE_SLACK) {
        print("  ERROR: failed huge allocations changed pool pages by %ld",
                  PagesDelta(after, before));
    }
                  
    ASSERT_FOOTER(test_name);
}

/* 3. Pool accounting: excess commitment (warning) and leaks across repeated cycles (fault) */
static BOOL TestAccountingAndLeaks(const char* test_name, BOOL has_tag)
{
    ASSERT_HEADER(test_name);
    
    void *blocks[64];
    ULONG n = 64, req = 1024, i, cycle, total, base, cycleBase, peak, after, expected, mm_ret;

    mm_ret = GetPoolPages(&base);
    GEN_CHECK(mm_ret == TRUE, TRUE, "MmQueryStatistics failed");
    if (mm_ret == FALSE) {
        ASSERT_FOOTER(test_name);
    }
    
    for (cycle = 0; cycle < CYCLES; cycle++) {
        mm_ret = GetPoolPages(&cycleBase);
        GEN_CHECK(mm_ret == TRUE, TRUE, "MmQueryStatistics failed");
        if (mm_ret == FALSE) {
            n = 64;
            continue;
        }
        total = 0;

        for (i = 0; i < n; i++) {
            blocks[i] = has_tag ? ExAllocatePoolWithTag(req, TEST_TAG) : ExAllocatePool(req);
            GEN_CHECK(blocks[i] != NULL, TRUE, "valid allocation must succeed");
            if (blocks[i] == NULL) {
                print("  ERROR: cycle %lu: alloc %lu failed at index %lu", cycle, req, i);
                n = i;
                break;
            }
            total += ExQueryPoolBlockSize(blocks[i]);
        }

        mm_ret = GetPoolPages(&peak);
        GEN_CHECK(mm_ret == TRUE, TRUE, "MmQueryStatistics failed");
        if (mm_ret == FALSE) {
            for (i = 0; i < n; i++) {
                ExFreePool(blocks[i]);
            }
            n = 64;
            continue;
        }
        expected = RoundUpPage(total) / PAGE_SIZE;
        if (PagesDelta(peak, cycleBase) > (LONG)(expected + PAGE_SLACK)) {
            print("  WARN: cycle %lu: %lu blocks of %lu bytes committed %ld pages, expected about %lu",
                      cycle, n, req, PagesDelta(peak, cycleBase), expected);
        }

        for (i = 0; i < n; i++) {
            ExFreePool(blocks[i]);
        }

        mm_ret = GetPoolPages(&after);
        GEN_CHECK(mm_ret == TRUE, TRUE, "MmQueryStatistics failed");
        if (mm_ret == FALSE) {
            n = 64;
            continue;
        }
        GEN_CHECK(PagesDelta(after, cycleBase) <= PAGE_SLACK, TRUE, "freed pages must return to the pool");
        if (PagesDelta(after, cycleBase) > PAGE_SLACK) {
            print("  ERROR: cycle %lu: leak, %ld pages not returned after freeing\n",
                      cycle, PagesDelta(after, cycleBase));
        }
        n = 64;
    }

    /* Catches slow leaks that per-cycle checks might miss */
    mm_ret = GetPoolPages(&after);
    GEN_CHECK(mm_ret == TRUE, TRUE, "MmQueryStatistics failed");
    if (mm_ret == FALSE) {
        ASSERT_FOOTER(test_name);
    }
    GEN_CHECK(PagesDelta(after, base) <= PAGE_SLACK, TRUE, "freed pages must return to the pool");
    if (PagesDelta(after, base) > PAGE_SLACK) {
        print("  ERROR: cumulative leak over %d cycles: %ld pages\n",
                  CYCLES, PagesDelta(after, base));
    }
    
    ASSERT_FOOTER(test_name);
}

/* 4. Random sizes, unique fill patterns, and pairwise overlap checks */
static BOOL TestOverlapAndIntegrity(const char* test_name, BOOL has_tag)
{
    ASSERT_HEADER(test_name);
    
    UCHAR *p[MAX_BLOCKS];
    ULONG bs[MAX_BLOCKS];
    ULONG count = 0, i, j, k;
    srand(seed);

    for (i = 0; i < MAX_BLOCKS; i++) {
        ULONG req, blk;

        req = 1 + (rand() % 2048);

        p[i] = has_tag ? (UCHAR *)ExAllocatePoolWithTag(req, TEST_TAG) : (UCHAR *)ExAllocatePool(req);
        if (p[i] == NULL) {
            print("  INFO: stopped at block %lu (pool exhausted or failed)", i);
            break;
        }
        blk = ExQueryPoolBlockSize(p[i]);
        if (blk < req) {
            TEST_FAILED();
            print("  ERROR: block %lu: size %lu < request %lu", i, blk, req);
        }
        bs[i] = blk;
        count++;

        for (j = 0; j < blk; j++) {
            p[i][j] = (UCHAR)(i + j);
        }
    }

    /* Each block must still hold its own pattern after all allocations */
    for (i = 0; i < count; i++) {
        for (j = 0; j < bs[i]; j++) {
            if (p[i][j] != (UCHAR)(i + j)) {
                TEST_FAILED();
                print("  ERROR: block %lu corrupted at offset %lu (overlap or heap damage)", i, j);
                break;
            }
        }
    }

    /* Pairwise check that reported ranges are disjoint */
    for (i = 0; i < count; i++) {
        for (k = i + 1; k < count; k++) {
            if ((p[i] < (p[k] + bs[k])) && (p[k] < (p[i] + bs[i]))) {
                TEST_FAILED();
                print("  ERROR: blocks %lu and %lu overlap", i, k);
            }
        }
    }

    for (i = 0; i < count; i++) {
        ExFreePool(p[i]);
    }
    
    ASSERT_FOOTER(test_name);
}

/* 5. Growth limit: 4 KB blocks should cost about one page each, and failure must return NULL */
static BOOL TestGrowthLimit(const char* test_name, BOOL has_tag)
{
    ASSERT_HEADER(test_name);
    
    static void *blocks[MAX_EXHAUST];
    ULONG base, after, count = 0, i, mm_ret;
    LONG delta;

    mm_ret = GetPoolPages(&base);
    GEN_CHECK(mm_ret == TRUE, TRUE, "MmQueryStatistics failed");
    if (mm_ret == FALSE) {
        ASSERT_FOOTER(test_name);
    }

    for (i = 0; i < MAX_EXHAUST; i++) {
        blocks[i] = has_tag ? ExAllocatePoolWithTag(PAGE_SIZE, TEST_TAG) : ExAllocatePool(PAGE_SIZE);
        if (blocks[i] == NULL) {
            break;
        }
        count++;
    }
    
    mm_ret = GetPoolPages(&after);
    GEN_CHECK(mm_ret == TRUE, TRUE, "MmQueryStatistics failed");
    if (mm_ret == FALSE) {
        for (i = 0; i < count; i++) {
            ExFreePool(blocks[i]);
        }
        ASSERT_FOOTER(test_name);
    }
    delta = PagesDelta(after, base);

    print("  INFO: allocated %lu x 4 KB blocks, pool pages +%ld", count, delta);
    if ((delta < ((LONG)count - PAGE_SLACK)) || (delta > ((LONG)count + PAGE_SLACK))) {
        print("  WARN: %lu blocks of 4 KB changed pool pages by %ld (expected about %lu)",
                  count, delta, count);
    }

    for (i = 0; i < count; i++) {
        ExFreePool(blocks[i]);
    }

    mm_ret = GetPoolPages(&after);
    GEN_CHECK(mm_ret == TRUE, TRUE, "MmQueryStatistics failed");
    if (mm_ret == FALSE) {
        ASSERT_FOOTER(test_name);
    }
    GEN_CHECK(PagesDelta(after, base) <= PAGE_SLACK, TRUE, "released pool must not leak memory");
    if (PagesDelta(after, base) > PAGE_SLACK) {
        print("  WARN: ERROR after growth test: %ld pages", PagesDelta(after, base));
    }
    
    ASSERT_FOOTER(test_name);
}

/* 6. Freed holes should be reused instead of committing new pages */
static BOOL TestHoleReuse(const char* test_name, BOOL has_tag)
{
    ASSERT_HEADER(test_name);
    
    static void *blocks[256];
    ULONG base, afterFree, afterRealloc, last, i, j, mm_ret;

    mm_ret = GetPoolPages(&base);
    GEN_CHECK(mm_ret == TRUE, TRUE, "MmQueryStatistics failed");
    if (mm_ret == FALSE) {
        ASSERT_FOOTER(test_name);
    }

    for (i = 0; i < 256; i++) {
        blocks[i] = has_tag ? ExAllocatePoolWithTag(128, TEST_TAG) : ExAllocatePool(128);
        GEN_CHECK(blocks[i] != NULL, TRUE, "valid allocation must succeed");
        if (blocks[i] == NULL) {
            print("  ERROR: hole test: alloc %lu failed", i);
            for (j = 0; j < i; j++) {
                ExFreePool(blocks[j]);
            }
            ASSERT_FOOTER(test_name);
        }
    }

    /* Free every second block to create holes */
    for (i = 0; i < 256; i += 2) {
        ExFreePool(blocks[i]);
        blocks[i] = NULL;
    }
    mm_ret = GetPoolPages(&afterFree);
    GEN_CHECK(mm_ret == TRUE, TRUE, "MmQueryStatistics failed");
    if (mm_ret == FALSE) {
        for (i = 1; i < 256; i += 2) {
            ExFreePool(blocks[i]);
        }
        ASSERT_FOOTER(test_name);
    }

    /* Refill the holes with the same size class */
    for (i = 0; i < 256; i += 2) {
        blocks[i] = has_tag ? ExAllocatePoolWithTag(128, TEST_TAG) : ExAllocatePool(128);
        GEN_CHECK(blocks[i] != NULL, TRUE, "valid allocation must succeed");
        if (blocks[i] == NULL) {
            print("  ERROR: hole test: refill %lu failed", i);
            for (i = 0; i < 256; i++) {
                if (blocks[i] != NULL) {
                    ExFreePool(blocks[i]);
                }
            }
            ASSERT_FOOTER(test_name);
        }
    }
    mm_ret = GetPoolPages(&afterRealloc);
    GEN_CHECK(mm_ret == TRUE, TRUE, "MmQueryStatistics failed");
    if (mm_ret == FALSE) {
        for (i = 0; i < 256; i++) {
            if (blocks[i] != NULL) {
                ExFreePool(blocks[i]);
            }
        }
        ASSERT_FOOTER(test_name);
    }

    if (PagesDelta(afterRealloc, afterFree) > PAGE_SLACK) {
        print("  WARN: poor reuse, refilling holes committed %ld extra pages",
                  PagesDelta(afterRealloc, afterFree));
    }

    for (i = 0; i < 256; i++) {
        if (blocks[i] != NULL) {
            ExFreePool(blocks[i]);
        }
    }

    mm_ret = GetPoolPages(&last);
    GEN_CHECK(mm_ret == TRUE, TRUE, "MmQueryStatistics failed");
    if (mm_ret == FALSE) {
        ASSERT_FOOTER(test_name);
    }
    GEN_CHECK(PagesDelta(last, base) <= PAGE_SLACK, TRUE, "pool memory must not leak");
    if (PagesDelta(last, base) > PAGE_SLACK) {
        print("  ERROR: leak after hole test");
    }
    
    ASSERT_FOOTER(test_name);
}

/* Test 7: the reported block size must be at least the requested size. */
static BOOL TestBlockSizeForRequests(const char* test_name)
{
    ASSERT_HEADER(test_name);
    
    static const ULONG sizes[] = {
        1, 7, 8, 9, 16, 31, 32, 33, 100, 255, 256,
        1000, 1024, 2048, 4000, 4096, 4097, 8192
    };
    ULONG i;

    for (i = 0; i < ARRAY_SIZE(sizes); i++) {
        PVOID block;
        ULONG blockSize;

        block = ExAllocatePoolWithTag(sizes[i], TEST_TAG);
        GEN_CHECK(block != NULL, TRUE, "valid allocation must succeed");
        if (block == NULL) {
            print("  ERROR: request of %u bytes failed", sizes[i]);
            continue;
        }

        /* Touch the full requested range to confirm the block is usable. */
        memset(block, 0xAA, sizes[i]);

        blockSize = ExQueryPoolBlockSize(block);
        print("  ExQueryPoolBlockSize = %u for %u bytes", blockSize, sizes[i]);

        GEN_CHECK(blockSize >= sizes[i], TRUE, "block size must be equal or larger than requested size");
        if (blockSize < sizes[i]) {
            print("  ERROR: block size >= requested size");
        }

        ExFreePool(block);
    }
    
    ASSERT_FOOTER(test_name);
}

/* Test 8: PoolPagesCommitted should not drop while allocations are live,
 * should not drop when blocks are allocated, and should not rise on free. */
static BOOL TestPoolPagesCommitted(const char* test_name)
{
    ASSERT_HEADER(test_name);
    
    PVOID blocks[STRESS_BLOCKS];
    ULONG before = 0, during = 0, afterFree = 0;
    ULONG allocated = 0, i;
    BOOLEAN mm_ret;

    mm_ret = GetPoolPages(&before);
    GEN_CHECK(mm_ret == TRUE, TRUE, "MmQueryStatistics failed");
    if (mm_ret == FALSE) {
        ASSERT_FOOTER(test_name);
    }

    for (i = 0; i < STRESS_BLOCKS; i++) {
        blocks[i] = ExAllocatePoolWithTag(PAGE_SIZE, TEST_TAG);
        if (blocks[i] == NULL) {
            break;
        }
        memset(blocks[i], 0, PAGE_SIZE);
        allocated++;
    }
    print("  allocated %u stress blocks of %u bytes", allocated, PAGE_SIZE);

    mm_ret = GetPoolPages(&during);
    GEN_CHECK(mm_ret == TRUE, TRUE, "MmQueryStatistics failed");
    if (mm_ret == FALSE) {
        for (i = 0; i < allocated; i++) {
            ExFreePool(blocks[i]);
        }
        ASSERT_FOOTER(test_name);
    }
    
    GEN_CHECK(during >= before, TRUE, "PoolPagesCommitted did not shrink while allocating");
    if (during < before) {
        print("  ERROR: PoolPagesCommitted before=%u during=%u", before, during);
        for (i = 0; i < allocated; i++) {
            ExFreePool(blocks[i]);
        }
        ASSERT_FOOTER(test_name);
    }

    /* Each block must be at least a full page, so the block size should be
     * at least PAGE_SIZE. */
    for (i = 0; i < allocated; i++) {
        ULONG size = ExQueryPoolBlockSize(blocks[i]);
        GEN_CHECK(size >= PAGE_SIZE, TRUE, "allocated pool memory must be at least a full page");
        if (size < PAGE_SIZE) {
            print("  ERROR: block %u reports size %u", i, size);
            break;
        }
    }

    for (i = 0; i < allocated; i++) {
        ExFreePool(blocks[i]);
    }

    mm_ret = GetPoolPages(&afterFree);
    GEN_CHECK(mm_ret == TRUE, TRUE, "MmQueryStatistics failed");
    if (mm_ret == FALSE) {
        ASSERT_FOOTER(test_name);
    }

    /* Freeing must not increase the committed count. The pool may keep
     * pages committed, so the count is not required to return to 'before'. */
    GEN_CHECK(afterFree <= during, TRUE, "PoolPagesCommitted did not grow on free");
    
    ASSERT_FOOTER(test_name);
}

TEST_FUNC(ExAllocatePool)
{
    TEST_BEGIN();
    
    test_fn fn_arr[] = {
        TestBasicSizes,
        TestInvalidSizes,
        TestAccountingAndLeaks,
        TestOverlapAndIntegrity,
        TestGrowthLimit,
        TestHoleReuse
    };
    char api_name_i[0x20];
    for (unsigned i = 0; i < ARRAY_SIZE(fn_arr); i++) {
        snprintf(api_name_i, ARRAY_SIZE(api_name_i), "%s[%u]", TEST_GET_API_NAME, i);
        TEST_GET_VAR &= fn_arr[i](api_name_i, FALSE);
    }
    HasTestedPoolAlloc = TRUE;
    
    TEST_END();
}

TEST_FUNC(ExAllocatePoolWithTag)
{
    TEST_BEGIN();
    
    test_fn fn_arr[] = {
        TestBasicSizes,
        TestInvalidSizes,
        TestAccountingAndLeaks,
        TestOverlapAndIntegrity,
        TestGrowthLimit,
        TestHoleReuse
    };
    char api_name_i[0x20];
    for (unsigned i = 0; i < ARRAY_SIZE(fn_arr); i++) {
        snprintf(api_name_i, ARRAY_SIZE(api_name_i), "%s[%u]", TEST_GET_API_NAME, i);
        TEST_GET_VAR &= fn_arr[i](api_name_i, TRUE);
    }
    HasTestedPoolAlloc = TRUE;
    
    TEST_END();
}

TEST_FUNC(ExFreePool)
{
    TEST_BEGIN();
    
    /* ExFreePool is implicitly tested by ExAllocatePool(WithTag), so skip the tests if we tested those already */
    if (HasTestedPoolAlloc == TRUE) {
        print("  Skipping test as ExFreePool is already implicitly tested by ExAllocatePool(WithTag) tests");
    }
    else {
        test_fn fn_arr[] = {
            TestBasicSizes,
            TestInvalidSizes,
            TestAccountingAndLeaks,
            TestOverlapAndIntegrity,
            TestGrowthLimit,
            TestHoleReuse
        };
        char api_name_i[0x20];
        for (unsigned i = 0; i < ARRAY_SIZE(fn_arr); i++) {
            snprintf(api_name_i, ARRAY_SIZE(api_name_i), "%s[%u]", TEST_GET_API_NAME, i);
            TEST_GET_VAR &= fn_arr[i](api_name_i, TRUE);
        }
    }
    
    TEST_END();
}
    
TEST_FUNC(ExQueryPoolBlockSize)
{
    TEST_BEGIN();
    
    TEST_GET_VAR &= TestBlockSizeForRequests("ExQueryPoolBlockSize[0]");
    TEST_GET_VAR &= TestPoolPagesCommitted("ExQueryPoolBlockSize[1]");
    
    TEST_END();
}
