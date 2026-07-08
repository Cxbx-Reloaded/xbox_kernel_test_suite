#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ObfReferenceObject)
{
    TEST_BEGIN();

    // ObfReferenceObject increments an object's PointerCount.
    // We use a kernel event object as a known-good object.

    HANDLE event_handle = NULL;
    NTSTATUS status = NtCreateEvent(&event_handle, NULL, NotificationEvent, FALSE);
    GEN_CHECK(status, STATUS_SUCCESS, "create event");

    if (NT_SUCCESS(status)) {
        // Get the object pointer from the handle
        PVOID object = NULL;
        status = ObReferenceObjectByHandle(event_handle, &ExEventObjectType, &object);
        GEN_CHECK(status, STATUS_SUCCESS, "get object pointer");

        if (NT_SUCCESS(status) && object != NULL) {
            // Read initial pointer count from the object header
            POBJECT_HEADER header = OBJECT_TO_OBJECT_HEADER(object);
            LONG initial_count = header->PointerCount;

            // Reference the object again
            ObfReferenceObject(object);
            LONG after_ref = header->PointerCount;
            GEN_CHECK(after_ref, initial_count + 1, "PointerCount incremented");

            // Reference again
            ObfReferenceObject(object);
            LONG after_ref2 = header->PointerCount;
            GEN_CHECK(after_ref2, initial_count + 2, "PointerCount incremented twice");

            // Balance: dereference the two extra references
            ObfDereferenceObject(object);
            ObfDereferenceObject(object);
            LONG after_deref = header->PointerCount;
            GEN_CHECK(after_deref, initial_count, "PointerCount restored");

            // Dereference the one from ObReferenceObjectByHandle
            ObfDereferenceObject(object);
        }

        NtClose(event_handle);
    }

    // SKIP: NULL object pointer - would crash (direct memory access to header).
    // SKIP: Freed/invalid object - undefined behavior, memory corruption.

    TEST_END();
}
