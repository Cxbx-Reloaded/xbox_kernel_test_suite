#pragma once

#include <xboxkrnl/ntstatus.h> // for STATUS_ prefix

static LARGE_INTEGER infinite = { .QuadPart = ((LONGLONG)INFINITE) * -10000 };
static LARGE_INTEGER thread_duo_wait_delay1 = { .QuadPart = ((LONGLONG)1) * -10000 };
static LARGE_INTEGER thread_duo_wait_delay10 = { .QuadPart = ((LONGLONG)10) * -10000 };

// Create two events for two threads switching
#define THREAD_DUO_EVENTS_CREATE(Primary, Secondary, InitialStatePrimary) \
    (void)NtCreateEvent(&Primary, NULL, SynchronizationEvent, InitialStatePrimary); \
    (void)NtCreateEvent(&Secondary, NULL, SynchronizationEvent, FALSE)
// Close event handles for two threads switching
#define THREAD_DUO_EVENTS_DESTROY(Primary, Secondary) \
    (void)NtClose(Secondary); \
    (void)NtClose(Primary)
// Wait until thread is terminated
#define THREAD_DUO_EVENTS_DESTROY_THREAD_WAIT(hThread) \
    (void)NtWaitForSingleObject(hThread, FALSE, &infinite); \
    (void)NtClose(hThread)
// Wait for event to trigger
#define THREAD_DUO_EVENTS_WAIT(data, hEvent, status) \
    do { \
        status = NtWaitForSingleObject(hEvent, FALSE, &thread_duo_wait_delay10); \
        if (status == STATUS_WAIT_0) { \
            break; \
        } \
        if (data->terminate) { \
            return 0; \
        } \
        if (status != STATUS_TIMEOUT) { \
            print("  %s thread terminated with 0x%08X", __func__ , status); \
            data->terminate = TRUE; \
            return 1; \
        } \
        (void)NtYieldExecution(); \
    } while (status == STATUS_TIMEOUT)
// Wait for event to trigger with status return only
#define THREAD_DUO_EVENTS_WAIT_GET_STATUS(hEvent, status) \
        status = NtWaitForSingleObject(hEvent, FALSE, &thread_duo_wait_delay1)
// Notify other thread to run
#define THREAD_DUO_EVENTS_SET(Notify) \
    (void)NtSetEvent(Notify, NULL); \
    /* Allow the other thread that may in need to process */ \
    (void)NtYieldExecution()
