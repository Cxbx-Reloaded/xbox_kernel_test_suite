#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(NtCreateEvent)
{
    TEST_BEGIN();

    typedef struct {
        const char* description;
        EVENT_TYPE event_type;
        BOOLEAN initial_state;
        NTSTATUS expected_status;
    } event_test_entry;

    event_test_entry tests[] = {
        { "notification, not signaled",    NotificationEvent,      FALSE, STATUS_SUCCESS },
        { "notification, signaled",        NotificationEvent,      TRUE,  STATUS_SUCCESS },
        { "synchronization, not signaled", SynchronizationEvent,   FALSE, STATUS_SUCCESS },
        { "synchronization, signaled",     SynchronizationEvent,   TRUE,  STATUS_SUCCESS },
    };
    size_t num_tests = ARRAY_SIZE(tests);

    for (unsigned i = 0; i < num_tests; i++) {
        HANDLE handle = NULL;
        NTSTATUS status = NtCreateEvent(&handle, NULL, tests[i].event_type, tests[i].initial_state);
        GEN_CHECK(status, tests[i].expected_status, tests[i].description);

        if (NT_SUCCESS(status)) {
            GEN_CHECK(handle != NULL, TRUE, tests[i].description);

            // Query to verify state matches creation parameters
            EVENT_BASIC_INFORMATION info;
            status = NtQueryEvent(handle, &info);
            GEN_CHECK(status, STATUS_SUCCESS, tests[i].description);
            GEN_CHECK(info.EventType, tests[i].event_type, tests[i].description);
            GEN_CHECK(info.EventState, (LONG)tests[i].initial_state, tests[i].description);

            NtClose(handle);
        }
    }

    // --- Named event (with OBJECT_ATTRIBUTES) ---
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\NtCreateEventTest");
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    HANDLE named_handle = NULL;
    NTSTATUS status = NtCreateEvent(&named_handle, &obj_attr, NotificationEvent, FALSE);
    if (NT_SUCCESS(status)) {
        GEN_CHECK(named_handle != NULL, TRUE, "named event created");
        NtClose(named_handle);
    }

    TEST_END();
}
