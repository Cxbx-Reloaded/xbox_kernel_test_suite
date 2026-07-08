#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(ExfInterlockedRemoveHeadList)
{
    TEST_BEGIN();

    LIST_ENTRY list_head;
    LIST_ENTRY entry1, entry2, entry3, entry4;

    InitializeListHead(&list_head);

    // --- Remove from empty list returns NULL ---
    PLIST_ENTRY removed = ExfInterlockedRemoveHeadList(&list_head);
    GEN_CHECK(removed == NULL, TRUE, "empty list returns NULL");

    // --- Verify list head is still valid after remove on empty ---
    GEN_CHECK(list_head.Flink, &list_head, "head still self-referencing Flink");
    GEN_CHECK(list_head.Blink, &list_head, "head still self-referencing Blink");

    // --- Insert 3 entries, remove in FIFO order ---
    ExfInterlockedInsertTailList(&list_head, &entry1);
    ExfInterlockedInsertTailList(&list_head, &entry2);
    ExfInterlockedInsertTailList(&list_head, &entry3);

    removed = ExfInterlockedRemoveHeadList(&list_head);
    GEN_CHECK(removed, &entry1, "first remove is entry1");

    removed = ExfInterlockedRemoveHeadList(&list_head);
    GEN_CHECK(removed, &entry2, "second remove is entry2");

    // --- Verify remaining list integrity after partial removal ---
    GEN_CHECK(list_head.Flink, &entry3, "entry3 is now head");
    GEN_CHECK(list_head.Blink, &entry3, "entry3 is also tail (only entry)");
    GEN_CHECK(entry3.Flink, &list_head, "entry3->Flink is head");
    GEN_CHECK(entry3.Blink, &list_head, "entry3->Blink is head");

    removed = ExfInterlockedRemoveHeadList(&list_head);
    GEN_CHECK(removed, &entry3, "third remove is entry3");

    // --- Empty again ---
    removed = ExfInterlockedRemoveHeadList(&list_head);
    GEN_CHECK(removed == NULL, TRUE, "empty after all removed");

    // --- Insert at head, remove from head (LIFO if using InsertHead) ---
    ExfInterlockedInsertHeadList(&list_head, &entry1);
    ExfInterlockedInsertHeadList(&list_head, &entry2);
    ExfInterlockedInsertHeadList(&list_head, &entry3);
    // Order is now: head -> entry3 -> entry2 -> entry1

    removed = ExfInterlockedRemoveHeadList(&list_head);
    GEN_CHECK(removed, &entry3, "head-inserted: remove gets entry3 (LIFO)");

    removed = ExfInterlockedRemoveHeadList(&list_head);
    GEN_CHECK(removed, &entry2, "head-inserted: remove gets entry2");

    removed = ExfInterlockedRemoveHeadList(&list_head);
    GEN_CHECK(removed, &entry1, "head-inserted: remove gets entry1");

    // --- Interleaved insert and remove ---
    ExfInterlockedInsertTailList(&list_head, &entry1);
    ExfInterlockedInsertTailList(&list_head, &entry2);
    removed = ExfInterlockedRemoveHeadList(&list_head);
    GEN_CHECK(removed, &entry1, "interleave: first remove");

    ExfInterlockedInsertTailList(&list_head, &entry3);
    ExfInterlockedInsertTailList(&list_head, &entry4);
    // List: head -> entry2 -> entry3 -> entry4

    removed = ExfInterlockedRemoveHeadList(&list_head);
    GEN_CHECK(removed, &entry2, "interleave: second remove");

    removed = ExfInterlockedRemoveHeadList(&list_head);
    GEN_CHECK(removed, &entry3, "interleave: third remove");

    removed = ExfInterlockedRemoveHeadList(&list_head);
    GEN_CHECK(removed, &entry4, "interleave: fourth remove");

    removed = ExfInterlockedRemoveHeadList(&list_head);
    GEN_CHECK(removed == NULL, TRUE, "interleave: empty");

    TEST_END();
}
