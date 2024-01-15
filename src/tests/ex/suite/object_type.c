#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/object_type.h"

TEST_FUNC(ExEventObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&ExEventObjectType, 'vevE', FALSE, FALSE, FALSE);

    TEST_END();
}

TEST_FUNC(ExMutantObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&ExMutantObjectType, 'atuM', FALSE, TRUE, FALSE);

    TEST_END();
}

TEST_FUNC(ExSemaphoreObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&ExSemaphoreObjectType, 'ameS', FALSE, FALSE, FALSE);

    TEST_END();
}

TEST_FUNC(ExTimerObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&ExTimerObjectType, 'emiT', FALSE, TRUE, FALSE);

    TEST_END();
}
