#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(IoCheckShareAccess)
{
    TEST_BEGIN();

    FILE_OBJECT file_obj;
    SHARE_ACCESS share_access;
    NTSTATUS status;

    memset(&file_obj, 0, sizeof(file_obj));
    memset(&share_access, 0, sizeof(share_access));

    // Set up initial share access: reader sharing read
    IoSetShareAccess(FILE_READ_DATA, FILE_SHARE_READ, &file_obj, &share_access);
    GEN_CHECK(share_access.OpenCount, 1, "OpenCount after set");
    GEN_CHECK(share_access.Readers, 1, "Readers after set");
    GEN_CHECK(share_access.SharedRead, 1, "SharedRead after set");

    // Check: another reader sharing read should succeed
    FILE_OBJECT file_obj2;
    memset(&file_obj2, 0, sizeof(file_obj2));
    status = IoCheckShareAccess(FILE_READ_DATA, FILE_SHARE_READ, &file_obj2, &share_access, TRUE);
    GEN_CHECK(status, STATUS_SUCCESS, "check reader+share_read");
    GEN_CHECK(share_access.OpenCount, 2, "OpenCount after check+update");
    GEN_CHECK(share_access.Readers, 2, "Readers after check+update");

    // Check: writer with no share should fail (existing reader requires FILE_SHARE_READ)
    FILE_OBJECT file_obj3;
    memset(&file_obj3, 0, sizeof(file_obj3));
    status = IoCheckShareAccess(FILE_WRITE_DATA, 0, &file_obj3, &share_access, FALSE);
    GEN_CHECK(status, STATUS_SHARING_VIOLATION, "check writer no share");

    TEST_END();
}
