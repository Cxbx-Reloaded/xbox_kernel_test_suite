#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/object_type.h"

TEST_FUNC(IoCompletionObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&IoCompletionObjectType, 'pmoC', FALSE, TRUE, FALSE);

    TEST_END();
}

TEST_FUNC(IoDeviceObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&IoDeviceObjectType, 'iveD', FALSE, FALSE, TRUE);

    TEST_END();
}

TEST_FUNC(IoFileObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&IoFileObjectType, 'eliF', TRUE, TRUE, TRUE);

    TEST_END();
}
