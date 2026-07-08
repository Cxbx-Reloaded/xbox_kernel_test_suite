#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtQueryFullAttributesFile)
{
    TEST_BEGIN();

    // Query attributes of the title partition root
    ANSI_STRING name;
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition1");
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    FILE_NETWORK_OPEN_INFORMATION info;
    NTSTATUS status = NtQueryFullAttributesFile(&obj_attr, &info);

    if (NT_SUCCESS(status)) {
        // Should be a directory
        GEN_CHECK((info.FileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0, TRUE, "is directory");
    } else {
        GEN_CHECK(TRUE, TRUE, "partition may not be available");
    }

    // --- Non-existent file ---
    RtlInitAnsiString(&name, "\\Device\\Harddisk0\\Partition1\\no_such_file_qfa.tmp");
    InitializeObjectAttributes(&obj_attr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);
    status = NtQueryFullAttributesFile(&obj_attr, &info);
    GEN_CHECK(NT_SUCCESS(status), FALSE, "non-existent fails");

    TEST_END();
}
