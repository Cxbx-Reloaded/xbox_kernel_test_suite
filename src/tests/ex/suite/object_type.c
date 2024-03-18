#include <xboxkrnl/xboxkrnl.h>
#include <synchapi.h>
#include <handleapi.h>

#include "util/output.h"
#include "assertions/object_type.h"

TEST_FUNC(ExEventObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&ExEventObjectType, 'vevE', FALSE, FALSE, FALSE);
    HANDLE event_handle = CreateEventA(NULL, TRUE, FALSE, "ExEventObjectType");
    GEN_CHECK(event_handle != INVALID_HANDLE_VALUE, TRUE, "event_handle");
    if (event_handle != INVALID_HANDLE_VALUE) {
        test_passed &= assert_object_header_type(&ExEventObjectType, event_handle);
        BOOL close = CloseHandle(event_handle);
        GEN_CHECK(close, TRUE, "close handle");
    }

    TEST_END();
}

TEST_FUNC(ExMutantObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&ExMutantObjectType, 'atuM', FALSE, TRUE, FALSE);
    HANDLE mutant_handle = CreateMutexA(NULL, TRUE, "ExMutantObjectType");
    GEN_CHECK(mutant_handle != INVALID_HANDLE_VALUE, TRUE, "mutant_handle");
    if (mutant_handle != INVALID_HANDLE_VALUE) {
        test_passed &= assert_object_header_type(&ExMutantObjectType, mutant_handle);
        BOOL close = CloseHandle(mutant_handle);
        GEN_CHECK(close, TRUE, "close handle");
    }

    TEST_END();
}

TEST_FUNC(ExSemaphoreObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&ExSemaphoreObjectType, 'ameS', FALSE, FALSE, FALSE);
    HANDLE semaphore_handle = CreateSemaphore(NULL, 0, 1, "ExSemaphoreObjectType");
    GEN_CHECK(semaphore_handle != INVALID_HANDLE_VALUE, TRUE, "semaphore_handle");
    if (semaphore_handle != INVALID_HANDLE_VALUE) {
        test_passed &= assert_object_header_type(&ExSemaphoreObjectType, semaphore_handle);
        BOOL close = CloseHandle(semaphore_handle);
        GEN_CHECK(close, TRUE, "close handle");
    }

    TEST_END();
}

TEST_FUNC(ExTimerObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&ExTimerObjectType, 'emiT', FALSE, TRUE, FALSE);
    // NOTE: There's no such thing as CreateTimer for Windows, it is done through SetTimer.
    ANSI_STRING obj_name;
    RtlInitAnsiString(&obj_name, "ExTimerObjectType");
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &obj_name, OBJ_OPENIF, ObWin32NamedObjectsDirectory(), NULL);
    // --------
    HANDLE timer_handle;
    NTSTATUS status = NtCreateTimer(&timer_handle, &obj_attr, NotificationTimer);
    GEN_CHECK(timer_handle != INVALID_HANDLE_VALUE, TRUE, "timer_handle");
    if (timer_handle != INVALID_HANDLE_VALUE) {
        test_passed &= assert_object_header_type(&ExTimerObjectType, timer_handle);
        BOOL close = CloseHandle(timer_handle);
        GEN_CHECK(close, TRUE, "close handle");
    }

    TEST_END();
}
