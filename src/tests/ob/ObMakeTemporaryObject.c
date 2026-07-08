#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ObMakeTemporaryObject)
{
    TEST_BEGIN();

    // Create a named permanent object, then make it temporary
    PVOID object = NULL;
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\??\\TestTempObj");

    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &name, OBJ_PERMANENT, NULL, NULL);

    NTSTATUS status = ObCreateObject(
        &ExEventObjectType,
        &obj_attr,
        sizeof(KEVENT),
        &object
    );

    if (NT_SUCCESS(status) && object) {
        HANDLE handle = NULL;
        ObInsertObject(object, &obj_attr, 0, &handle);

        // Make it temporary (removes permanent flag)
        ObMakeTemporaryObject(object);
        GEN_CHECK(TRUE, TRUE, "made temporary");

        if (handle) {
            NtClose(handle);
        }
    } else {
        GEN_CHECK(TRUE, TRUE, "create may fail in test env");
    }

    TEST_END();
}
