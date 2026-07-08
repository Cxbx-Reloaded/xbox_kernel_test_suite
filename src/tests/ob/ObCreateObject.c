#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ObCreateObject)
{
    TEST_BEGIN();

    // Create an event object through ObCreateObject
    PVOID object = NULL;
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, NULL, 0, NULL, NULL);

    NTSTATUS status = ObCreateObject(
        &ExEventObjectType,
        &obj_attr,
        sizeof(KEVENT),
        &object
    );
    GEN_CHECK(status, STATUS_SUCCESS, "create object");
    GEN_CHECK(object != NULL, TRUE, "object not null");

    if (NT_SUCCESS(status) && object) {
        // Object was created but not inserted into handle table
        // We need to dereference it
        ObfDereferenceObject(object);
    }

    TEST_END();
}
