#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(IoRemoveShareAccess)
{
    TEST_BEGIN();

    FILE_OBJECT file_obj1, file_obj2;
    SHARE_ACCESS share_access;

    memset(&file_obj1, 0, sizeof(file_obj1));
    memset(&file_obj2, 0, sizeof(file_obj2));
    memset(&share_access, 0, sizeof(share_access));

    // Add first opener: read access, share read
    IoSetShareAccess(FILE_READ_DATA, FILE_SHARE_READ, &file_obj1, &share_access);
    GEN_CHECK(share_access.OpenCount, 1, "OpenCount after first");

    // Add second opener via IoCheckShareAccess with Update=TRUE
    NTSTATUS status = IoCheckShareAccess(FILE_READ_DATA, FILE_SHARE_READ, &file_obj2, &share_access, TRUE);
    GEN_CHECK(status, STATUS_SUCCESS, "check second reader");
    GEN_CHECK(share_access.OpenCount, 2, "OpenCount after second");
    GEN_CHECK(share_access.Readers, 2, "Readers after second");

    // Remove second opener
    IoRemoveShareAccess(&file_obj2, &share_access);
    GEN_CHECK(share_access.OpenCount, 1, "OpenCount after remove");
    GEN_CHECK(share_access.Readers, 1, "Readers after remove");
    GEN_CHECK(share_access.SharedRead, 1, "SharedRead after remove");

    // Remove first opener
    IoRemoveShareAccess(&file_obj1, &share_access);
    GEN_CHECK(share_access.OpenCount, 0, "OpenCount after remove all");
    GEN_CHECK(share_access.Readers, 0, "Readers after remove all");

    TEST_END();
}
