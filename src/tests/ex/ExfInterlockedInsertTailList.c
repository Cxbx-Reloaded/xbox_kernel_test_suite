#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ExfInterlockedInsertTailList)
{
    TEST_BEGIN();

    LIST_ENTRY list_head;
    LIST_ENTRY entry1, entry2, entry3, entry4;

    InitializeListHead(&list_head);

    // --- First insert into empty list returns NULL (no previous tail) ---
    PLIST_ENTRY prev = ExfInterlockedInsertTailList(&list_head, &entry1);
    GEN_CHECK(prev == NULL, TRUE, "first insert returns NULL (was empty)");

    // --- Subsequent inserts return previous tail ---
    prev = ExfInterlockedInsertTailList(&list_head, &entry2);
    GEN_CHECK(prev == &entry1, TRUE, "second insert returns previous tail");

    prev = ExfInterlockedInsertTailList(&list_head, &entry3);
    GEN_CHECK(prev == &entry2, TRUE, "third insert returns previous tail");

    // --- Verify forward links: head -> entry1 -> entry2 -> entry3 -> head ---
    GEN_CHECK(list_head.Flink, &entry1, "head->Flink is entry1");
    GEN_CHECK(entry1.Flink, &entry2, "entry1->Flink is entry2");
    GEN_CHECK(entry2.Flink, &entry3, "entry2->Flink is entry3");
    GEN_CHECK(entry3.Flink, &list_head, "entry3->Flink is head (circular)");

    // --- Verify backward links: head <- entry1 <- entry2 <- entry3 <- head ---
    GEN_CHECK(list_head.Blink, &entry3, "head->Blink is entry3 (tail)");
    GEN_CHECK(entry3.Blink, &entry2, "entry3->Blink is entry2");
    GEN_CHECK(entry2.Blink, &entry1, "entry2->Blink is entry1");
    GEN_CHECK(entry1.Blink, &list_head, "entry1->Blink is head");

    // --- Remove from middle, then insert at tail again ---
    RemoveEntryList(&entry2);
    GEN_CHECK(entry1.Flink, &entry3, "after remove: entry1->Flink is entry3");
    GEN_CHECK(entry3.Blink, &entry1, "after remove: entry3->Blink is entry1");

    // Re-insert entry2 at tail
    prev = ExfInterlockedInsertTailList(&list_head, &entry2);
    GEN_CHECK(prev == &entry3, TRUE, "re-insert at tail returns entry3");
    GEN_CHECK(list_head.Blink, &entry2, "entry2 is new tail");
    GEN_CHECK(entry2.Blink, &entry3, "entry2->Blink is entry3");

    // --- Insert 4th entry; verify full chain ---
    prev = ExfInterlockedInsertTailList(&list_head, &entry4);
    GEN_CHECK(prev == &entry2, TRUE, "4th insert returns entry2");
    // Final order: head -> entry1 -> entry3 -> entry2 -> entry4 -> head
    GEN_CHECK(list_head.Flink, &entry1, "final: head->Flink entry1");
    GEN_CHECK(entry1.Flink, &entry3, "final: entry1->Flink entry3");
    GEN_CHECK(entry3.Flink, &entry2, "final: entry3->Flink entry2");
    GEN_CHECK(entry2.Flink, &entry4, "final: entry2->Flink entry4");
    GEN_CHECK(entry4.Flink, &list_head, "final: entry4->Flink head");

    TEST_END();
}
