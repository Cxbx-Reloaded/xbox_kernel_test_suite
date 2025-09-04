#pragma once

#include <synchapi.h>
#include <handleapi.h>

// Create two events for two threads switching
#define THREAD_DUO_EVENTS_CREATE(Primary, Secondary) \
    HANDLE Primary = CreateEventA(NULL, TRUE, TRUE, #Primary); \
    HANDLE Secondary = CreateEventA(NULL, TRUE, FALSE, #Secondary)
// Close event handles for two threads switching
#define THREAD_DUO_EVENTS_DESTROY(Primary, Secondary) \
    (void)CloseHandle(Secondary); \
    (void)CloseHandle(Primary)
// Wait for primary thread event is trigger
#define THREAD_DUO_EVENTS_WAIT_PRIMARY(Primary) \
    WaitForSingleObject(Primary, 1)
// Wait for secondary thread event is trigger
#define THREAD_DUO_EVENTS_WAIT_SECONDARY(Secondary) \
    (void)WaitForSingleObject(Secondary, INFINITE)
// Notify other thread to run (if possible)
#define THREAD_DUO_EVENTS_NOTIFY(Reset, Notify) \
    (void)SetEvent(Notify); \
    (void)ResetEvent(Reset); \
    (void)NtYieldExecution()
