#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

static KDPC dpcObject;
static BOOL dpc_called;
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
    /* FIXME: This is a stub! implement this function! */
}

void test_KeIsExecutingDpc()
{
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRemoveQueueDpc()
{
    /* FIXME: This is a stub! implement this function! */
}
