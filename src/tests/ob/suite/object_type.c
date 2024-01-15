#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/object_type.h"

TEST_FUNC(ObDirectoryObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&ObDirectoryObjectType, 'eriD', FALSE, FALSE, FALSE);

    TEST_END();
}

TEST_FUNC(ObSymbolicLinkObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&ObSymbolicLinkObjectType, 'bmyS', FALSE, TRUE, FALSE);

    TEST_END();
}
