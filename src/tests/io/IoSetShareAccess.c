#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(IoSetShareAccess)
{
    TEST_BEGIN();

    FILE_OBJECT file_obj;
    SHARE_ACCESS share_access;

    memset(&file_obj, 0, sizeof(file_obj));
    memset(&share_access, 0, sizeof(share_access));

    // Set share access with read access and share read+write
    IoSetShareAccess(FILE_READ_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, &file_obj, &share_access);
    GEN_CHECK(share_access.OpenCount, 1, "OpenCount");
    GEN_CHECK(share_access.Readers, 1, "Readers");
    GEN_CHECK(share_access.Writers, 0, "Writers");
    GEN_CHECK(share_access.SharedRead, 1, "SharedRead");
    GEN_CHECK(share_access.SharedWrite, 1, "SharedWrite");
    GEN_CHECK(share_access.SharedDelete, 0, "SharedDelete");

    // Verify file object flags were set
    GEN_CHECK((ULONG)file_obj.ReadAccess, TRUE, "file_obj.ReadAccess");
    GEN_CHECK((ULONG)file_obj.WriteAccess, FALSE, "file_obj.WriteAccess");
    GEN_CHECK((ULONG)file_obj.SharedRead, TRUE, "file_obj.SharedRead");
    GEN_CHECK((ULONG)file_obj.SharedWrite, TRUE, "file_obj.SharedWrite");

    // Set with write access and share delete
    memset(&file_obj, 0, sizeof(file_obj));
    memset(&share_access, 0, sizeof(share_access));
    IoSetShareAccess(FILE_WRITE_DATA, FILE_SHARE_DELETE, &file_obj, &share_access);
    GEN_CHECK(share_access.Writers, 1, "Writers after write");
    GEN_CHECK(share_access.Readers, 0, "Readers after write");
    GEN_CHECK(share_access.SharedDelete, 1, "SharedDelete");
    GEN_CHECK((ULONG)file_obj.WriteAccess, TRUE, "file_obj.WriteAccess write");

    TEST_END();
}
