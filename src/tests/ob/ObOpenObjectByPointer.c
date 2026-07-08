#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ObOpenObjectByPointer)
{
    TEST_BEGIN();

    // ObOpenObjectByPointer creates a handle for a given object pointer.

    HANDLE event_handle = NULL;
    NTSTATUS status = NtCreateEvent(&event_handle, NULL, NotificationEvent, FALSE);
    GEN_CHECK(status, STATUS_SUCCESS, "create event");

    if (NT_SUCCESS(status)) {
        PVOID object = NULL;
        status = ObReferenceObjectByHandle(event_handle, &ExEventObjectType, &object);
        GEN_CHECK(status, STATUS_SUCCESS, "get object pointer");

        if (NT_SUCCESS(status) && object != NULL) {
            POBJECT_HEADER header = OBJECT_TO_OBJECT_HEADER(object);
            LONG initial_handle_count = header->HandleCount;

            // --- Open a new handle to the same object ---
            HANDLE new_handle = NULL;
            status = ObOpenObjectByPointer(object, &ExEventObjectType, &new_handle);
            GEN_CHECK(status, STATUS_SUCCESS, "open by pointer succeeds");
            GEN_CHECK(new_handle != NULL, TRUE, "got new handle");

            // Handle count should have increased
            LONG after_open = header->HandleCount;
            GEN_CHECK(after_open, initial_handle_count + 1, "handle count +1");

            // The new handle should be different from the original
            GEN_CHECK(new_handle != event_handle, TRUE, "different handle value");

            // Can use the new handle to reference the same object
            PVOID object2 = NULL;
            status = ObReferenceObjectByHandle(new_handle, &ExEventObjectType, &object2);
            GEN_CHECK(status, STATUS_SUCCESS, "new handle is valid");
            GEN_CHECK(object2, object, "same object body");
            if (object2) ObfDereferenceObject(object2);

            // Close the new handle
            NtClose(new_handle);
            LONG after_close = header->HandleCount;
            GEN_CHECK(after_close, initial_handle_count, "handle count restored");

            // --- NULL object type (should fail - unlike ObReferenceObjectByHandle,
            //     ObReferenceObjectByPointer does NOT accept NULL as "any type") ---
            HANDLE null_type_handle = NULL;
            status = ObOpenObjectByPointer(object, NULL, &null_type_handle);
            GEN_CHECK(status, STATUS_OBJECT_TYPE_MISMATCH, "NULL type fails");
            GEN_CHECK(null_type_handle == NULL, TRUE, "no handle on NULL type");

            // --- Wrong object type ---
            HANDLE bad_handle = NULL;
            status = ObOpenObjectByPointer(object, &ObSymbolicLinkObjectType, &bad_handle);
            GEN_CHECK(status, STATUS_OBJECT_TYPE_MISMATCH, "wrong type fails");

            ObfDereferenceObject(object);
        }

        NtClose(event_handle);
    }

    TEST_END();
}
