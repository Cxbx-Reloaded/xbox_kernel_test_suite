#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ObReferenceObjectByPointer)
{
    TEST_BEGIN();

    // ObReferenceObjectByPointer increments the reference count and optionally
    // validates the object type.

    HANDLE event_handle = NULL;
    NTSTATUS status = NtCreateEvent(&event_handle, NULL, NotificationEvent, FALSE);
    GEN_CHECK(status, STATUS_SUCCESS, "create event");

    if (NT_SUCCESS(status)) {
        PVOID object = NULL;
        status = ObReferenceObjectByHandle(event_handle, &ExEventObjectType, &object);
        GEN_CHECK(status, STATUS_SUCCESS, "get object pointer");

        if (NT_SUCCESS(status) && object != NULL) {
            POBJECT_HEADER header = OBJECT_TO_OBJECT_HEADER(object);
            LONG initial_count = header->PointerCount;

            // --- Correct object type ---
            status = ObReferenceObjectByPointer(object, &ExEventObjectType);
            GEN_CHECK(status, STATUS_SUCCESS, "correct type succeeds");
            LONG after_ref = header->PointerCount;
            GEN_CHECK(after_ref, initial_count + 1, "refcount incremented");

            // --- NULL object type (should fail) ---
            status = ObReferenceObjectByPointer(object, NULL);
            GEN_CHECK(status, STATUS_OBJECT_TYPE_MISMATCH, "NULL type fails");
            LONG after_null = header->PointerCount;
            GEN_CHECK(after_null, initial_count + 1, "refcount unchanged on NULL type");

            // --- Wrong object type ---
            status = ObReferenceObjectByPointer(object, &ObSymbolicLinkObjectType);
            GEN_CHECK(status, STATUS_OBJECT_TYPE_MISMATCH, "wrong type fails");
            LONG after_wrong = header->PointerCount;
            GEN_CHECK(after_wrong, initial_count + 1, "refcount unchanged on type mismatch");

            // Balance
            ObfDereferenceObject(object);
            ObfDereferenceObject(object); // from ObReferenceObjectByHandle
        }

        NtClose(event_handle);
    }

    TEST_END();
}
