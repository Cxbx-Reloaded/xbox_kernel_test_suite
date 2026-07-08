#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ObInsertObject)
{
    TEST_BEGIN();

    // Create an object and insert it into the handle table
    PVOID object = NULL;
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, NULL, 0, NULL, NULL);

    NTSTATUS status = ObCreateObject(
        &ExEventObjectType,
        &obj_attr,
        sizeof(KEVENT),
        &object
    );
    GEN_CHECK(status, STATUS_SUCCESS, "create for insert");

    if (NT_SUCCESS(status) && object) {
        HANDLE handle = NULL;
        status = ObInsertObject(object, &obj_attr, 0, &handle);
        GEN_CHECK(status, STATUS_SUCCESS, "insert object");
        GEN_CHECK(handle != NULL, TRUE, "got handle");

        if (handle) {
            NtClose(handle);
        }
    }

    TEST_END();
}
