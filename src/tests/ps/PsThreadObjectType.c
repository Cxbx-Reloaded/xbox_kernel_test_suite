#include <xboxkrnl/xboxkrnl.h>
#include <processthreadsapi.h>
#include <synchapi.h>
#include <handleapi.h>

#include "util/output.h"
#include "assertions/object_type.h"

DWORD NTAPI dummy_thread(
    IN PVOID context
)
{
    return 0;
}

TEST_FUNC(PsThreadObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&PsThreadObjectType, 'erhT', FALSE, FALSE, FALSE);
    HANDLE thread_handle = CreateThread(NULL, 0, dummy_thread, NULL, 0, NULL);
    GEN_CHECK(thread_handle != INVALID_HANDLE_VALUE, TRUE, "thread_handle");
    if (thread_handle != INVALID_HANDLE_VALUE) {
        test_passed &= assert_object_header_type(&PsThreadObjectType, thread_handle);
        do {
            DWORD ExitCode = 0;
            GetExitCodeThread(thread_handle, &ExitCode);
            if (ExitCode != STILL_ACTIVE) {
                break;
            }
            Sleep(10);
        }
        while(1);
        BOOL close = CloseHandle(thread_handle);
        GEN_CHECK(close, TRUE, "close handle");
    }

    TEST_END();
}
