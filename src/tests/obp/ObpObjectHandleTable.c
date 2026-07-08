#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ObpObjectHandleTable)
{
    TEST_BEGIN();

    // ObpObjectHandleTable is the global kernel object handle table.
    // It's an exported variable of type OBJECT_HANDLE_TABLE.

    // The handle table should have a positive handle count (kernel always has handles open)
    GEN_CHECK(ObpObjectHandleTable.HandleCount > 0, TRUE, "HandleCount > 0");

    // RootTable should be non-NULL (initialized at boot)
    GEN_CHECK(ObpObjectHandleTable.RootTable != NULL, TRUE, "RootTable not NULL");

    // Create a handle and verify the count increases
    LONG before_count = ObpObjectHandleTable.HandleCount;

    HANDLE event_handle = NULL;
    NTSTATUS status = NtCreateEvent(&event_handle, NULL, NotificationEvent, FALSE);
    GEN_CHECK(status, STATUS_SUCCESS, "create event");

    if (NT_SUCCESS(status)) {
        LONG after_count = ObpObjectHandleTable.HandleCount;
        GEN_CHECK(after_count, before_count + 1, "HandleCount incremented");

        // Create another handle
        HANDLE event_handle2 = NULL;
        status = NtCreateEvent(&event_handle2, NULL, NotificationEvent, FALSE);
        if (NT_SUCCESS(status)) {
            LONG after_count2 = ObpObjectHandleTable.HandleCount;
            GEN_CHECK(after_count2, before_count + 2, "HandleCount +2");
            NtClose(event_handle2);
        }

        // Close the first handle
        NtClose(event_handle);
        LONG after_close = ObpObjectHandleTable.HandleCount;
        GEN_CHECK(after_close, before_count, "HandleCount restored after close");
    }

    TEST_END();
}
