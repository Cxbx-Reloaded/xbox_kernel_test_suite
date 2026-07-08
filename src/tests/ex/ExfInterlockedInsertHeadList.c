#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ExfInterlockedInsertHeadList)
{
    TEST_BEGIN();

    LIST_ENTRY list_head;
    LIST_ENTRY entry1, entry2, entry3;

    InitializeListHead(&list_head);

    // --- Return value on empty list is NULL ---
    PLIST_ENTRY prev = ExfInterlockedInsertHeadList(&list_head, &entry1);
    GEN_CHECK(prev == NULL, TRUE, "first insert returns NULL (was empty)");

    // --- Return value is previous first entry ---
    prev = ExfInterlockedInsertHeadList(&list_head, &entry2);
    GEN_CHECK(prev == &entry1, TRUE, "second insert returns previous head");

    prev = ExfInterlockedInsertHeadList(&list_head, &entry3);
    GEN_CHECK(prev == &entry2, TRUE, "third insert returns previous head");

    // --- Verify forward links: head -> entry3 -> entry2 -> entry1 -> head ---
    GEN_CHECK(list_head.Flink, &entry3, "head->Flink is entry3");
    GEN_CHECK(entry3.Flink, &entry2, "entry3->Flink is entry2");
    GEN_CHECK(entry2.Flink, &entry1, "entry2->Flink is entry1");
    GEN_CHECK(entry1.Flink, &list_head, "entry1->Flink is head (circular)");

    // --- Verify backward links: head <- entry3 <- entry2 <- entry1 <- head ---
    GEN_CHECK(list_head.Blink, &entry1, "head->Blink is entry1 (tail)");
    GEN_CHECK(entry1.Blink, &entry2, "entry1->Blink is entry2");
    GEN_CHECK(entry2.Blink, &entry3, "entry2->Blink is entry3");
    GEN_CHECK(entry3.Blink, &list_head, "entry3->Blink is head");

    // --- Verify list integrity after removing an entry ---
    // Remove entry2 from the middle
    RemoveEntryList(&entry2);
    GEN_CHECK(entry3.Flink, &entry1, "after remove: entry3->Flink is entry1");
    GEN_CHECK(entry1.Blink, &entry3, "after remove: entry1->Blink is entry3");

    // --- Insert after removal to verify list still works ---
    prev = ExfInterlockedInsertHeadList(&list_head, &entry2);
    GEN_CHECK(prev == &entry3, TRUE, "re-insert returns current head (entry3)");
    GEN_CHECK(list_head.Flink, &entry2, "re-inserted entry2 is new head");

    TEST_END();
}
