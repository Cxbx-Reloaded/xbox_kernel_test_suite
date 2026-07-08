#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtDuplicateObject)
{
    TEST_BEGIN();

    // Create an event to duplicate
    HANDLE original = NULL;
    NTSTATUS status = NtCreateEvent(&original, NULL, NotificationEvent, FALSE);
    GEN_CHECK(status, STATUS_SUCCESS, "create event");

    if (NT_SUCCESS(status)) {
        HANDLE duplicate = NULL;
        ULONG options = 0; // DUPLICATE_SAME_ACCESS equivalent
        status = NtDuplicateObject(original, &duplicate, options);
        GEN_CHECK(status, STATUS_SUCCESS, "duplicate succeeds");
        GEN_CHECK(duplicate != NULL, TRUE, "got dup handle");
        GEN_CHECK(duplicate != original, TRUE, "different handle value");

        if (NT_SUCCESS(status)) {
            // Both handles should reference the same object
            // Signal via original, check via duplicate
            NtSetEvent(original, NULL);

            LARGE_INTEGER timeout;
            timeout.QuadPart = 0;
            status = NtWaitForSingleObject(duplicate, FALSE, &timeout);
            GEN_CHECK(status, STATUS_SUCCESS, "dup sees signal");

            NtClose(duplicate);
        }

        // --- Duplicate with DUPLICATE_CLOSE_SOURCE ---
        HANDLE dup2 = NULL;
        status = NtDuplicateObject(original, &dup2, DUPLICATE_CLOSE_SOURCE);
        GEN_CHECK(status, STATUS_SUCCESS, "dup+close source");

        if (NT_SUCCESS(status)) {
            // Original handle should now be invalid
            LARGE_INTEGER timeout;
            timeout.QuadPart = 0;
            status = NtWaitForSingleObject(original, FALSE, &timeout);
            GEN_CHECK(status, STATUS_INVALID_HANDLE, "original closed");

            NtClose(dup2);
        }
    }

    TEST_END();
}
