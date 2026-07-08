#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ObfDereferenceObject)
{
    TEST_BEGIN();

    // ObfDereferenceObject decrements an object's PointerCount.
    // When PointerCount and HandleCount both reach 0, the object is freed.

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

            // Add extra references so we can safely dereference
            ObfReferenceObject(object);
            ObfReferenceObject(object);
            LONG after_2refs = header->PointerCount;
            GEN_CHECK(after_2refs, initial_count + 2, "2 refs added");

            // Dereference once
            ObfDereferenceObject(object);
            LONG after_1deref = header->PointerCount;
            GEN_CHECK(after_1deref, initial_count + 1, "1 deref");

            // Dereference again
            ObfDereferenceObject(object);
            LONG after_2deref = header->PointerCount;
            GEN_CHECK(after_2deref, initial_count, "2 derefs back to initial");

            // Balance the ObReferenceObjectByHandle reference
            ObfDereferenceObject(object);
        }

        NtClose(event_handle);
    }

    // SKIP: Dereferencing to 0 with handle still open - kernel handles that
    //       internally, but deliberately triggering it could corrupt state.
    // SKIP: NULL object - would crash.

    TEST_END();
}
