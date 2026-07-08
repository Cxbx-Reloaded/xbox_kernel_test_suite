#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeRundownQueue)
{
    TEST_BEGIN();

    KQUEUE queue;
    LIST_ENTRY entry1, entry2;

    KeInitializeQueue(&queue, 1);
    KeInsertQueue(&queue, &entry1);
    KeInsertQueue(&queue, &entry2);

    // Rundown returns list of remaining entries
    PLIST_ENTRY list = KeRundownQueue(&queue);
    GEN_CHECK(list != NULL, TRUE, "entries returned");

    TEST_END();
}
