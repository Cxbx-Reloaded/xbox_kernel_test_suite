#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/object_type.h"

TEST_FUNC(PsThreadObjectType)
{
    TEST_BEGIN();

    test_passed &= assert_object_type(&PsThreadObjectType, 'erhT', FALSE, FALSE, FALSE);

    TEST_END();
}
