#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

// TODO: Add list of _LEVEL suffix support into nxdk
#define DPC_DISPATCH_LEVEL 2 // DISPATCH_LEVEL

typedef struct _dpc_test {
    BOOLEAN inserted;
    BOOL dpc_called;
    KDPC dpcObject;
} dpc_test;

static ULONG getDpcRoutineActive()
{
    ULONG getter;
    __asm__ ("movl %%fs:0x58, %0"
            : "=r"(getter));
    return getter;
}

static void setDpcRoutineActive(ULONG setter)
{
    __asm__ ("movl %0, %%fs:0x58"
            :
            : "a"(setter));
}

static KDPC dpcObject;
static BOOL dpc_called;
static ULONG dpc_executing;
static void __stdcall dpc_callback(
	IN KDPC* Dpc,
	IN PVOID DeferredContext,
	IN PVOID SystemArgument1,
	IN PVOID SystemArgument2
)
{
    // DPC called variable
    if (SystemArgument1 && (DWORD)SystemArgument1 > 0xFF) {
        *(BOOL*)SystemArgument1 = TRUE;
    }

    // Request KeIsExecutingDpc return value
    if (SystemArgument2 && (DWORD)SystemArgument2 > 0xFF) {
        *(ULONG*)SystemArgument2 = KeIsExecutingDpc();
    }
}

void test_KeInitializeDpc()
{
    const char* func_num = "0x006B";
    const char* func_name = "KeInitializeDpc";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    // Force set dpc object's variables as invalid to verify if KeInitializeDpc call does set them or not.
    // According to hardware, some are not set. Let's force check them anyway.
    memset(&dpcObject, 0xCC, sizeof(KDPC));

    KeInitializeDpc(&dpcObject, dpc_callback, &dpc_called);

    // Inspect each member variables are correctly setup.
    GEN_CHECK(dpcObject.Type, 0x13/*DpcObject*/, "dpcObject.Type");
    GEN_CHECK(dpcObject.Inserted, FALSE, "dpcObject.Inserted");
    GEN_CHECK(dpcObject.DeferredRoutine, dpc_callback, "dpcObject.DeferredRoutine");
    GEN_CHECK(dpcObject.DeferredContext, &dpc_called, "dpcObject.DeferredContext");
    // The rest of below are not set from KeInitializeDpc.
    GEN_CHECK(dpcObject.DpcListEntry.Blink, (PVOID)0xCCCCCCCC, "dpcObject.DpcListEntry.Blink");
    GEN_CHECK(dpcObject.DpcListEntry.Flink, (PVOID)0xCCCCCCCC, "dpcObject.DpcListEntry.Flink");
    GEN_CHECK(dpcObject.Padding, 0xCC, "dpcObject.Padding");
    GEN_CHECK(dpcObject.SystemArgument1, (PVOID)0xCCCCCCCC, "dpcObject.SystemArgument1");
    GEN_CHECK(dpcObject.SystemArgument2, (PVOID)0xCCCCCCCC, "dpcObject.SystemArgument2");

    // clear DPC object for later test request
    memset(&dpcObject, 0, sizeof(KDPC));

    print_test_footer(func_num, func_name, test_passed);
}

void test_KeInsertQueueDpc()
{
    const char* func_num = "0x0077";
    const char* func_name = "KeInsertQueueDpc";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    // Initialize test
    KeInitializeDpc(&dpcObject, dpc_callback, NULL);
    KDPC restore_dpc_object = dpcObject;

    // Test insertion & executed DPC within KeInsertQueueDpc function
    BOOLEAN inserted = KeInsertQueueDpc(&dpcObject, &dpc_called, (PVOID)0x20);
    GEN_CHECK(inserted, TRUE, "1:inserted");
    GEN_CHECK(dpc_called, TRUE, "1:dpc_called");
    GEN_CHECK(dpcObject.Inserted, FALSE, "1:dpcObject.Inserted");
    // Leftover storage, kernel doesn't reset any member variables except for Inserted variable.
    GEN_CHECK(dpcObject.SystemArgument1, &dpc_called, "1:dpcObject.SystemArgument1");
    GEN_CHECK(dpcObject.SystemArgument2, (PVOID)0x20, "1:dpcObject.SystemArgument2");

    // Reset test
    dpc_called = FALSE;
    dpcObject = restore_dpc_object;

    // Test as if DPC object had been inserted to receive proper response back.
    // HACK: Perform fake inserted which will not trigger DPC callback function.
    dpcObject.Inserted = TRUE;
    inserted = KeInsertQueueDpc(&dpcObject, (PVOID)0x30, (PVOID)0x40);
    GEN_CHECK(inserted, FALSE, "2:inserted");
    GEN_CHECK(dpc_called, FALSE, "2:dpc_called");
    GEN_CHECK(dpcObject.Inserted, TRUE, "2:dpcObject.Inserted");
    GEN_CHECK(dpcObject.SystemArgument1, NULL, "2:dpcObject.SystemArgument1");
    GEN_CHECK(dpcObject.SystemArgument2, NULL, "2:dpcObject.SystemArgument2");

    // Reset test
    dpc_called = FALSE;
    dpcObject = restore_dpc_object;

    ULONG orgDpcRoutineActive = getDpcRoutineActive();

    // Test as if DPC object is remain in queue line.
    // HACK: Disable internal trigger for DPC callback function.
    setDpcRoutineActive(1);
    dpc_test test3a;
    test3a.inserted = KeInsertQueueDpc(&dpcObject, &dpc_called, (PVOID)0x50);
    test3a.dpc_called = dpc_called;
    test3a.dpcObject = dpcObject;

    dpc_test test3b;
    test3b.inserted = KeInsertQueueDpc(&dpcObject, &dpc_called, NULL);
    test3b.dpc_called = dpc_called;
    test3b.dpcObject = dpcObject;

    // Restore and trigger dispatcher thread.
    setDpcRoutineActive(orgDpcRoutineActive);

    // Test without raise irql (expected not to trigger callback)
    HalRequestSoftwareInterrupt(DPC_DISPATCH_LEVEL);
    BOOL test3c_dpc_called = dpc_called;

    // Test with raise irql (expected to trigger callback)
    KIRQL oldIrql = KfRaiseIrql(DPC_DISPATCH_LEVEL);
    HalRequestSoftwareInterrupt(DPC_DISPATCH_LEVEL);
    BOOL test3d_dpc_called = dpc_called;
    KfLowerIrql(oldIrql);
    BOOL test3e_dpc_called = dpc_called;

    // If callback was not called, then it is a failure and need removal from queue.
    if (!dpc_called) {
        (void)KeRemoveQueueDpc(&dpcObject);
    }

    // Print all failure of #3 tests outside of unsafe codes
    GEN_CHECK(test3a.inserted, TRUE, "3a:inserted");
    GEN_CHECK(test3a.dpc_called, FALSE, "3a:dpc_called (before)");
    GEN_CHECK(test3a.dpcObject.Inserted, TRUE, "3a:dpcObject.Inserted");
    GEN_CHECK(test3a.dpcObject.SystemArgument1, &dpc_called, "3a:dpcObject.SystemArgument1");
    GEN_CHECK(test3a.dpcObject.SystemArgument2, (PVOID)0x50, "3a:dpcObject.SystemArgument2");

    GEN_CHECK(test3b.inserted, FALSE, "3b:inserted");
    GEN_CHECK(test3b.dpc_called, FALSE, "3b:dpc_called (before)");
    GEN_CHECK(test3b.dpcObject.Inserted, TRUE, "3b:dpcObject.Inserted");
    GEN_CHECK(test3b.dpcObject.SystemArgument1, &dpc_called, "3b:dpcObject.SystemArgument1");
    GEN_CHECK(test3b.dpcObject.SystemArgument2, (PVOID)0x50, "3b:dpcObject.SystemArgument2");

    GEN_CHECK(test3c_dpc_called, FALSE, "3c:dpc_called (after)");

    GEN_CHECK(test3d_dpc_called, FALSE, "3d:dpc_called (after)");

    GEN_CHECK(test3e_dpc_called, TRUE, "3e:dpc_called (after)");

    // Reset test
    dpc_called = FALSE;
    dpcObject = restore_dpc_object;

    print_test_footer(func_num, func_name, test_passed);
}

void test_KeIsExecutingDpc()
{
    const char* func_num = "0x0079";
    const char* func_name = "KeIsExecutingDpc";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    // Initialize test
    KeInitializeDpc(&dpcObject, dpc_callback, NULL);
    KDPC restore_dpc_object = dpcObject;

    // Test direct call without additional steps
    dpc_executing = KeIsExecutingDpc();
    GEN_CHECK(dpc_called, FALSE, "dpc_called");
    GEN_CHECK(dpc_executing, FALSE, "dpc_executing");

    // Reset test
    dpc_called = FALSE;
    dpc_executing = FALSE;

    // Test insertion & executed DPC callback within KeInsertQueueDpc function
    BOOLEAN inserted = KeInsertQueueDpc(&dpcObject, &dpc_called, &dpc_executing);
    GEN_CHECK(dpc_called, TRUE, "callback.dpc_called");
    GEN_CHECK((dpc_executing != FALSE), TRUE, "callback.dpc_executing");

    // Reset test
    dpc_called = FALSE;
    dpc_executing = FALSE;

    // Test manual raise to verify if it thinks is running in DPC part 1
    KIRQL oldIrql = KfRaiseIrql(DPC_DISPATCH_LEVEL);
    dpc_executing = KeIsExecutingDpc();
    GEN_CHECK(dpc_called, FALSE, "dpc_called");
    GEN_CHECK(dpc_executing, FALSE, "dpc_executing");
    KfLowerIrql(oldIrql);

    // Reset test
    dpc_called = FALSE;
    dpc_executing = FALSE;

    // Test manual raise to verify if it thinks is running in DPC part 2
    oldIrql = KeRaiseIrqlToDpcLevel();
    dpc_executing = KeIsExecutingDpc();
    GEN_CHECK(dpc_called, FALSE, "dpc_called");
    GEN_CHECK(dpc_executing, FALSE, "dpc_executing");
    KfLowerIrql(oldIrql);

    // Reset test
    dpc_called = FALSE;
    dpc_executing = FALSE;

    print_test_footer(func_num, func_name, test_passed);
}

void test_KeRemoveQueueDpc()
{
    /* FIXME: This is a stub! implement this function! */
}
