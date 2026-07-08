#include <xboxkrnl/xboxkrnl.h>
#include <windows.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

// Shared state for thread-safety contention tests
#define CONTENTION_ITERATIONS 10000

typedef struct _interlocked_thread_data {
    volatile LONG* shared_counter;
    LONG iterations;
    volatile LONG threads_ready;
    volatile LONG start_flag;
} interlocked_thread_data;

static void NTAPI increment_thread_func(void* arg)
{
    interlocked_thread_data* data = (interlocked_thread_data*)arg;
    InterlockedIncrement(&data->threads_ready);
    // Spin until start flag is set to maximize contention
    while (!data->start_flag) { }
    for (LONG i = 0; i < data->iterations; i++) {
        InterlockedIncrement(data->shared_counter);
    }
    PsTerminateSystemThread(STATUS_SUCCESS);
}

static void NTAPI decrement_thread_func(void* arg)
{
    interlocked_thread_data* data = (interlocked_thread_data*)arg;
    InterlockedIncrement(&data->threads_ready);
    while (!data->start_flag) { }
    for (LONG i = 0; i < data->iterations; i++) {
        InterlockedDecrement(data->shared_counter);
    }
    PsTerminateSystemThread(STATUS_SUCCESS);
}

static void NTAPI exchange_add_thread_func(void* arg)
{
    interlocked_thread_data* data = (interlocked_thread_data*)arg;
    InterlockedIncrement(&data->threads_ready);
    while (!data->start_flag) { }
    for (LONG i = 0; i < data->iterations; i++) {
        InterlockedExchangeAdd(data->shared_counter, 1);
    }
    PsTerminateSystemThread(STATUS_SUCCESS);
}

static void NTAPI cmpxchg_spinlock_thread_func(void* arg)
{
    interlocked_thread_data* data = (interlocked_thread_data*)arg;
    InterlockedIncrement(&data->threads_ready);
    while (!data->start_flag) { }
    // Use CompareExchange as a spinlock to safely increment a non-atomic counter
    // shared_counter[0] = spinlock, shared_counter[1] = protected value
    volatile LONG* lock = &data->shared_counter[0];
    volatile LONG* value = &data->shared_counter[1];
    for (LONG i = 0; i < data->iterations; i++) {
        // Acquire spinlock
        while (InterlockedCompareExchange((PLONG)lock, 1, 0) != 0) { }
        // Critical section - non-atomic increment of protected value
        (*value)++;
        // Release spinlock
        InterlockedExchange((PLONG)lock, 0);
    }
    PsTerminateSystemThread(STATUS_SUCCESS);
}

TEST_FUNC(InterlockedCompareExchange)
{
    TEST_BEGIN();

    typedef struct _cmpxchg_test {
        LONG initial;
        LONG exchange;
        LONG comparand;
        LONG expected_return;
        LONG expected_dest;
    } cmpxchg_test;

    cmpxchg_test tests[] = {
        // Match: comparand == initial, so exchange happens
        { .initial = 5, .exchange = 10, .comparand = 5, .expected_return = 5, .expected_dest = 10 },
        // No match: comparand != initial, so no exchange
        { .initial = 5, .exchange = 10, .comparand = 3, .expected_return = 5, .expected_dest = 5 },
        // Zero case
        { .initial = 0, .exchange = 1, .comparand = 0, .expected_return = 0, .expected_dest = 1 },
        // Negative values
        { .initial = -1, .exchange = 42, .comparand = -1, .expected_return = -1, .expected_dest = 42 },
        { .initial = -1, .exchange = 42, .comparand = 0, .expected_return = -1, .expected_dest = -1 },
    };

    for (unsigned i = 0; i < ARRAY_SIZE(tests); i++) {
        LONG dest = tests[i].initial;
        LONG ret = InterlockedCompareExchange(&dest, tests[i].exchange, tests[i].comparand);
        GEN_CHECK(ret, tests[i].expected_return, "return value");
        GEN_CHECK(dest, tests[i].expected_dest, "destination");
    }

    // Thread-safety test: Use CompareExchange as a spinlock to protect a
    // non-atomic increment. Two threads each increment CONTENTION_ITERATIONS times.
    // If CompareExchange is truly atomic, the spinlock works and no updates are lost.
    {
        volatile LONG shared[2] = { 0, 0 }; // [0]=spinlock, [1]=protected counter
        interlocked_thread_data data = {
            .shared_counter = shared,
            .iterations = CONTENTION_ITERATIONS,
            .threads_ready = 0,
            .start_flag = 0
        };
        HANDLE h1, h2;
        PsCreateSystemThread(&h1, NULL, cmpxchg_spinlock_thread_func, &data, 0);
        PsCreateSystemThread(&h2, NULL, cmpxchg_spinlock_thread_func, &data, 0);
        while (data.threads_ready < 2) { }
        InterlockedExchange((PLONG)&data.start_flag, 1);
        NtWaitForSingleObject(h1, FALSE, NULL);
        NtWaitForSingleObject(h2, FALSE, NULL);
        NtClose(h1);
        NtClose(h2);
        GEN_CHECK(shared[0], 0, "spinlock released");
        GEN_CHECK(shared[1], 2 * CONTENTION_ITERATIONS, "spinlock-protected counter");
    }

    TEST_END();
}

TEST_FUNC(InterlockedDecrement)
{
    TEST_BEGIN();

    typedef struct _dec_test {
        LONG initial;
        LONG expected_return;
    } dec_test;

    dec_test tests[] = {
        { .initial = 1, .expected_return = 0 },
        { .initial = 0, .expected_return = -1 },
        { .initial = -1, .expected_return = -2 },
        { .initial = 100, .expected_return = 99 },
        { .initial = (LONG)0x80000000, .expected_return = 0x7FFFFFFF }, // Underflow wrap
    };

    for (unsigned i = 0; i < ARRAY_SIZE(tests); i++) {
        LONG dest = tests[i].initial;
        LONG ret = InterlockedDecrement(&dest);
        GEN_CHECK(ret, tests[i].expected_return, "return value");
        GEN_CHECK(dest, tests[i].expected_return, "destination");
    }

    // Thread-safety test: Two threads each decrement CONTENTION_ITERATIONS times.
    // If atomic, final value = initial - 2*CONTENTION_ITERATIONS
    {
        volatile LONG shared = 2 * CONTENTION_ITERATIONS;
        interlocked_thread_data data = {
            .shared_counter = &shared,
            .iterations = CONTENTION_ITERATIONS,
            .threads_ready = 0,
            .start_flag = 0
        };
        HANDLE h1, h2;
        PsCreateSystemThread(&h1, NULL, decrement_thread_func, &data, 0);
        PsCreateSystemThread(&h2, NULL, decrement_thread_func, &data, 0);
        // Wait for both threads to be ready
        while (data.threads_ready < 2) { }
        // Release both simultaneously for maximum contention
        InterlockedExchange((PLONG)&data.start_flag, 1);
        NtWaitForSingleObject(h1, FALSE, NULL);
        NtWaitForSingleObject(h2, FALSE, NULL);
        NtClose(h1);
        NtClose(h2);
        GEN_CHECK(shared, 0, "concurrent decrement result");
    }

    TEST_END();
}

TEST_FUNC(InterlockedIncrement)
{
    TEST_BEGIN();

    typedef struct _inc_test {
        LONG initial;
        LONG expected_return;
    } inc_test;

    inc_test tests[] = {
        { .initial = 0, .expected_return = 1 },
        { .initial = -1, .expected_return = 0 },
        { .initial = 1, .expected_return = 2 },
        { .initial = 100, .expected_return = 101 },
        { .initial = 0x7FFFFFFF, .expected_return = (LONG)0x80000000 }, // Overflow wrap
    };

    for (unsigned i = 0; i < ARRAY_SIZE(tests); i++) {
        LONG dest = tests[i].initial;
        LONG ret = InterlockedIncrement(&dest);
        GEN_CHECK(ret, tests[i].expected_return, "return value");
        GEN_CHECK(dest, tests[i].expected_return, "destination");
    }

    // Thread-safety test: Two threads each increment CONTENTION_ITERATIONS times.
    // If atomic, final value = 2*CONTENTION_ITERATIONS
    {
        volatile LONG shared = 0;
        interlocked_thread_data data = {
            .shared_counter = &shared,
            .iterations = CONTENTION_ITERATIONS,
            .threads_ready = 0,
            .start_flag = 0
        };
        HANDLE h1, h2;
        PsCreateSystemThread(&h1, NULL, increment_thread_func, &data, 0);
        PsCreateSystemThread(&h2, NULL, increment_thread_func, &data, 0);
        while (data.threads_ready < 2) { }
        InterlockedExchange((PLONG)&data.start_flag, 1);
        NtWaitForSingleObject(h1, FALSE, NULL);
        NtWaitForSingleObject(h2, FALSE, NULL);
        NtClose(h1);
        NtClose(h2);
        GEN_CHECK(shared, 2 * CONTENTION_ITERATIONS, "concurrent increment result");
    }

    TEST_END();
}

TEST_FUNC(InterlockedExchange)
{
    TEST_BEGIN();

    typedef struct _xchg_test {
        LONG initial;
        LONG new_value;
        LONG expected_return;
    } xchg_test;

    xchg_test tests[] = {
        { .initial = 0, .new_value = 5, .expected_return = 0 },
        { .initial = 5, .new_value = 0, .expected_return = 5 },
        { .initial = -1, .new_value = 42, .expected_return = -1 },
        { .initial = 0x12345678, .new_value = (LONG)0xDEADBEEF, .expected_return = 0x12345678 },
    };

    for (unsigned i = 0; i < ARRAY_SIZE(tests); i++) {
        LONG dest = tests[i].initial;
        LONG ret = InterlockedExchange(&dest, tests[i].new_value);
        GEN_CHECK(ret, tests[i].expected_return, "return value");
        GEN_CHECK(dest, tests[i].new_value, "destination");
    }

    TEST_END();
}

TEST_FUNC(InterlockedExchangeAdd)
{
    TEST_BEGIN();

    typedef struct _xchgadd_test {
        LONG initial;
        LONG addend;
        LONG expected_return;
        LONG expected_dest;
    } xchgadd_test;

    xchgadd_test tests[] = {
        { .initial = 0, .addend = 5, .expected_return = 0, .expected_dest = 5 },
        { .initial = 10, .addend = -3, .expected_return = 10, .expected_dest = 7 },
        { .initial = -5, .addend = -5, .expected_return = -5, .expected_dest = -10 },
        { .initial = 0x7FFFFFFF, .addend = 1, .expected_return = 0x7FFFFFFF, .expected_dest = (LONG)0x80000000 },
    };

    for (unsigned i = 0; i < ARRAY_SIZE(tests); i++) {
        LONG dest = tests[i].initial;
        LONG ret = InterlockedExchangeAdd(&dest, tests[i].addend);
        GEN_CHECK(ret, tests[i].expected_return, "return value");
        GEN_CHECK(dest, tests[i].expected_dest, "destination");
    }

    // Thread-safety test: Two threads each add 1, CONTENTION_ITERATIONS times.
    // If atomic, final value = 2*CONTENTION_ITERATIONS
    {
        volatile LONG shared = 0;
        interlocked_thread_data data = {
            .shared_counter = &shared,
            .iterations = CONTENTION_ITERATIONS,
            .threads_ready = 0,
            .start_flag = 0
        };
        HANDLE h1, h2;
        PsCreateSystemThread(&h1, NULL, exchange_add_thread_func, &data, 0);
        PsCreateSystemThread(&h2, NULL, exchange_add_thread_func, &data, 0);
        while (data.threads_ready < 2) { }
        InterlockedExchange((PLONG)&data.start_flag, 1);
        NtWaitForSingleObject(h1, FALSE, NULL);
        NtWaitForSingleObject(h2, FALSE, NULL);
        NtClose(h1);
        NtClose(h2);
        GEN_CHECK(shared, 2 * CONTENTION_ITERATIONS, "concurrent ExchangeAdd result");
    }

    TEST_END();
}

TEST_FUNC(InterlockedFlushSList)
{
    TEST_BEGIN();

    // Initialize an SList head
    SLIST_HEADER list_head;
    list_head.Alignment = 0; // Initializes Depth=0, Next.Next=NULL

    // Flushing an empty list should return NULL
    PSINGLE_LIST_ENTRY result = InterlockedFlushSList(&list_head);
    GEN_CHECK(result, NULL, "flush empty list");

    // Push some entries then flush
    SLIST_ENTRY entry1, entry2, entry3;
    InterlockedPushEntrySList(&list_head, &entry1);
    InterlockedPushEntrySList(&list_head, &entry2);
    InterlockedPushEntrySList(&list_head, &entry3);

    // Flush should return pointer to first (most recently pushed) entry
    result = InterlockedFlushSList(&list_head);
    GEN_CHECK(result, (PSINGLE_LIST_ENTRY)&entry3, "flush returns head");

    // After flush, list should be empty
    result = InterlockedPopEntrySList(&list_head);
    GEN_CHECK(result, NULL, "list empty after flush");

    TEST_END();
}

TEST_FUNC(InterlockedPopEntrySList)
{
    TEST_BEGIN();

    SLIST_HEADER list_head;
    list_head.Alignment = 0;

    // Pop from empty list returns NULL
    PSINGLE_LIST_ENTRY result = InterlockedPopEntrySList(&list_head);
    GEN_CHECK(result, NULL, "pop empty list");

    // Push one entry and pop it
    SLIST_ENTRY entry1;
    InterlockedPushEntrySList(&list_head, &entry1);
    result = InterlockedPopEntrySList(&list_head);
    GEN_CHECK(result, (PSINGLE_LIST_ENTRY)&entry1, "pop single entry");

    // List should now be empty again
    result = InterlockedPopEntrySList(&list_head);
    GEN_CHECK(result, NULL, "pop after single removed");

    // Push multiple, pop in LIFO order
    SLIST_ENTRY entry2, entry3;
    InterlockedPushEntrySList(&list_head, &entry1);
    InterlockedPushEntrySList(&list_head, &entry2);
    InterlockedPushEntrySList(&list_head, &entry3);

    result = InterlockedPopEntrySList(&list_head);
    GEN_CHECK(result, (PSINGLE_LIST_ENTRY)&entry3, "LIFO pop 1");
    result = InterlockedPopEntrySList(&list_head);
    GEN_CHECK(result, (PSINGLE_LIST_ENTRY)&entry2, "LIFO pop 2");
    result = InterlockedPopEntrySList(&list_head);
    GEN_CHECK(result, (PSINGLE_LIST_ENTRY)&entry1, "LIFO pop 3");

    TEST_END();
}

TEST_FUNC(InterlockedPushEntrySList)
{
    TEST_BEGIN();

    SLIST_HEADER list_head;
    list_head.Alignment = 0;

    SLIST_ENTRY entry1, entry2, entry3;

    // Push onto empty list returns NULL (previous head)
    PSINGLE_LIST_ENTRY result = InterlockedPushEntrySList(&list_head, &entry1);
    GEN_CHECK(result, NULL, "push to empty list");

    // Push second entry returns previous head (entry1)
    result = InterlockedPushEntrySList(&list_head, &entry2);
    GEN_CHECK(result, (PSINGLE_LIST_ENTRY)&entry1, "push returns prev head 1");

    // Push third entry returns previous head (entry2)
    result = InterlockedPushEntrySList(&list_head, &entry3);
    GEN_CHECK(result, (PSINGLE_LIST_ENTRY)&entry2, "push returns prev head 2");

    // Verify LIFO order by popping
    result = InterlockedPopEntrySList(&list_head);
    GEN_CHECK(result, (PSINGLE_LIST_ENTRY)&entry3, "verify LIFO 1");
    result = InterlockedPopEntrySList(&list_head);
    GEN_CHECK(result, (PSINGLE_LIST_ENTRY)&entry2, "verify LIFO 2");
    result = InterlockedPopEntrySList(&list_head);
    GEN_CHECK(result, (PSINGLE_LIST_ENTRY)&entry1, "verify LIFO 3");

    TEST_END();
}
