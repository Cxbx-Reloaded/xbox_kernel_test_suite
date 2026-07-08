#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(NtQueryEvent)
{
    TEST_BEGIN();

    typedef struct {
        const char* description;
        EVENT_TYPE type;
        BOOLEAN initial_state;
    } event_query_test;

    event_query_test tests[] = {
        { "notification non-signaled",    NotificationEvent,    FALSE },
        { "notification signaled",        NotificationEvent,    TRUE  },
        { "synchronization non-signaled", SynchronizationEvent, FALSE },
        { "synchronization signaled",     SynchronizationEvent, TRUE  },
    };
    size_t num_tests = ARRAY_SIZE(tests);

    for (unsigned i = 0; i < num_tests; i++) {
        HANDLE handle = NULL;
        NTSTATUS status = NtCreateEvent(&handle, NULL, tests[i].type, tests[i].initial_state);
        GEN_CHECK(status, STATUS_SUCCESS, tests[i].description);

        if (NT_SUCCESS(status)) {
            EVENT_BASIC_INFORMATION info;
            status = NtQueryEvent(handle, &info);
            GEN_CHECK(status, STATUS_SUCCESS, tests[i].description);
            GEN_CHECK(info.EventType, tests[i].type, tests[i].description);
            GEN_CHECK(info.EventState, (LONG)tests[i].initial_state, tests[i].description);
            NtClose(handle);
        }
    }

    // --- Invalid handle ---
    EVENT_BASIC_INFORMATION info;
    NTSTATUS status = NtQueryEvent((HANDLE)0xDEAD, &info);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "invalid handle");

    TEST_END();
}
