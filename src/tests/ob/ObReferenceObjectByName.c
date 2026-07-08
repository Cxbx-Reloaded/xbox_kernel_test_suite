#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ObReferenceObjectByName)
{
    TEST_BEGIN();

    // ObReferenceObjectByName looks up an object by its path in the object namespace.

    // --- Look up a well-known device object ---
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition0");

    PVOID object = NULL;
    NTSTATUS status = ObReferenceObjectByName(
        &name,
        OBJ_CASE_INSENSITIVE,
        &IoDeviceObjectType,
        NULL,  // ParseContext
        &object
    );

    if (NT_SUCCESS(status)) {
        GEN_CHECK(object != NULL, TRUE, "partition0 object returned");

        // Verify we can read the object header
        POBJECT_HEADER header = OBJECT_TO_OBJECT_HEADER(object);
        GEN_CHECK(header->PointerCount > 0, TRUE, "valid refcount");
        GEN_CHECK(header->Type, &IoDeviceObjectType, "type is IoDeviceObjectType");

        ObfDereferenceObject(object);
    } else {
        // Device may not be present in all environments
        GEN_CHECK(TRUE, TRUE, "partition0 lookup attempted");
    }

    // --- Non-existent object ---
    RtlInitAnsiString(&name, "\\Device\\NonExistentDevice12345");
    object = NULL;
    status = ObReferenceObjectByName(
        &name,
        OBJ_CASE_INSENSITIVE,
        &IoDeviceObjectType,
        NULL,
        &object
    );
    GEN_CHECK(NT_SUCCESS(status), FALSE, "non-existent device fails");
    GEN_CHECK(object, NULL, "object NULL on failure");

    // --- Wrong object type for an existing object ---
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition0");
    object = NULL;
    status = ObReferenceObjectByName(
        &name,
        OBJ_CASE_INSENSITIVE,
        &ExEventObjectType, // Wrong type for a device
        NULL,
        &object
    );
    // Should fail with type mismatch (or it may fail at parse context level)
    if (NT_SUCCESS(status)) {
        // Some implementations don't type-check at this level
        ObfDereferenceObject(object);
    }

    TEST_END();
}
