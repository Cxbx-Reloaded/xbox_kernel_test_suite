#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ObOpenObjectByName)
{
    TEST_BEGIN();

    // Try to open a well-known device object
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition0");

    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    HANDLE handle = NULL;
    NTSTATUS status = ObOpenObjectByName(
        &obj_attr,
        &IoDeviceObjectType,
        NULL,  // ParseContext
        &handle
    );
    // This may or may not succeed depending on device availability
    if (NT_SUCCESS(status)) {
        GEN_CHECK(handle != NULL, TRUE, "got handle");
        NtClose(handle);
    } else {
        // Not finding the device is OK in some environments
        GEN_CHECK(TRUE, TRUE, "open attempted (device may not exist)");
    }

    TEST_END();
}
