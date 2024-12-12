#include "tcb.h"
#include "test_utils.h"
#include <stdbool.h>

extern TCBError JOCKTOS_TCBError;

void testCreateTCB() {

    TEST_CASE("TCB creation with default values") {

        TaskControlBlock tcb = TASKCONTROLBLOCK_DEF();

        ASSERT_EQUAL_INT(tcb.priority, 0, "incorrect default priority after initialization");
        ASSERT_EQUAL_STR(tcb.name,"\0", 1, "incorrect default name after initialization");
        ASSERT_EQUAL_INT(tcb.delay_ms, 0, "incorrect default delay_ms after initialization");
        ASSERT_EQUAL_INT(tcb.stack_size_bytes, 0, "incorrect default stack size after initialization");
        ASSERT_EQUAL_PTR(tcb.task_handle, NULL, "incorrect default task function after initialization");
        ASSERT_EQUAL_PTR(tcb.task_arg, NULL, "incorrect default task argument after initialization");
        ASSERT_EQUAL_INT(tcb.state, BLOCKED, "incorrect default task state after initialization");
        ASSERT_EQUAL_PTR(tcb.stack_pointer, NULL, "incorrect default task stack pointer after initialization");
        ASSERT_EQUAL_PTR(tcb.next, NULL, "incorrect default next pointer after initialization");

    } CASE_COMPLETE;

    TEST_CASE("TCB creation with custom values") {
        
        TaskControlBlock tcb = TASKCONTROLBLOCK_DEF(
            .priority = 1,
            .name = "test",
            .delay_ms = 2,
            .stack_size_bytes = 3,
            .task_handle = ((void*)0x1234),
            .task_arg   = ((void*)0x2468),
            .state = READY,
            .stack_pointer = ((void*)0x5678),
            .next = ((void*)0x369a));

        ASSERT_EQUAL_INT(tcb.priority, 1, "incorrect priority after initialization");
        ASSERT_EQUAL_STR(tcb.name, "test", 4, "incorrect name after initialization");
        ASSERT_EQUAL_INT(tcb.delay_ms, 2, "incorrect delay_ms after initialization");
        ASSERT_EQUAL_INT(tcb.stack_size_bytes, 3, "incorrect stack size after initialization");
        ASSERT_EQUAL_PTR(tcb.task_handle, ((void*)0x1234), "incorrect task function after initialization");   
        ASSERT_EQUAL_PTR(tcb.task_arg, ((void*)0x2468), "incorrect task argument after initialization");
        ASSERT_EQUAL_INT(tcb.state, READY, "incorrect task state after initialization");
        ASSERT_EQUAL_PTR(tcb.stack_pointer, ((void*)0x5678), "incorrect task stack pointer after initialization");
        ASSERT_EQUAL_PTR(tcb.next, ((void*)0x369a), "incorrect next pointer after initialization");

    } CASE_COMPLETE;
}

void testInsertTCB() {

    TEST_CASE("insert into empty list") {

        volatile TaskControlBlock* head = NULL;
        volatile TaskControlBlock tcb = TASKCONTROLBLOCK_DEF();

        _insert_tcb(&head, &tcb);
        ASSERT_EQUAL_PTR(head, &tcb, "insertion at head of empty list failed");
        ASSERT_EQUAL_PTR(head->next, NULL, "tail of list was modified");

    } CASE_COMPLETE;

    TEST_CASE("insert at head of non-empty list") {

        volatile TaskControlBlock tcb0 = TASKCONTROLBLOCK_DEF(.priority = 0);
        volatile TaskControlBlock tcb1 = TASKCONTROLBLOCK_DEF(.priority = 1);
        volatile TaskControlBlock tcb2 = TASKCONTROLBLOCK_DEF(.priority = 2);

        volatile TaskControlBlock* head = &tcb1;
        tcb1.next = &tcb0;
        
        _insert_tcb(&head, &tcb2);
        ASSERT_EQUAL_PTR(head, &tcb2, "higher priority TCB not assigned to head");
        ASSERT_EQUAL_PTR(head->next, &tcb1, "previous head not assigned to next");
        ASSERT_EQUAL_PTR(head->next->next, &tcb0, "order of TCBs not preserved");
        ASSERT_EQUAL_PTR(head->next->next->next, NULL, "tail of list was modified");

    } CASE_COMPLETE;

    TEST_CASE("insert into middle of list") {

        volatile TaskControlBlock tcb0 = TASKCONTROLBLOCK_DEF(.priority = 0);
        volatile TaskControlBlock tcb1 = TASKCONTROLBLOCK_DEF(.priority = 1);
        volatile TaskControlBlock tcb2 = TASKCONTROLBLOCK_DEF(.priority = 2);

        volatile TaskControlBlock* head = &tcb2;
        tcb2.next = &tcb0;

        _insert_tcb(&head, &tcb1);
        ASSERT_EQUAL_PTR(head, &tcb2, "higher priority TCB not assigned to head");
        ASSERT_EQUAL_PTR(head->next, &tcb1, "previous head not assigned to next");
        ASSERT_EQUAL_PTR(head->next->next, &tcb0, "order of TCBs not preserved");
        ASSERT_EQUAL_PTR(head->next->next->next, NULL, "tail of list was modified");

    } CASE_COMPLETE;

    TEST_CASE("insert at tail of list") {

        volatile TaskControlBlock* head;
        volatile TaskControlBlock tcb0 = TASKCONTROLBLOCK_DEF(.priority = 0);
        volatile TaskControlBlock tcb1 = TASKCONTROLBLOCK_DEF(.priority = 1);
        volatile TaskControlBlock tcb2 = TASKCONTROLBLOCK_DEF(.priority = 2);

        head = &tcb2;
        tcb2.next = &tcb1;

        _insert_tcb(&head, &tcb0);
        ASSERT_EQUAL_PTR(head, &tcb2, "higher priority TCB not assigned to head");
        ASSERT_EQUAL_PTR(head->next, &tcb1, "order of TCBs not preserved");
        ASSERT_EQUAL_PTR(head->next->next, &tcb0, "lower priority TCB not assigned to tail");
        ASSERT_EQUAL_PTR(head->next->next->next, NULL, "tail of list was modified");

    } CASE_COMPLETE;

    TEST_CASE("insertion of duplicate TCB priority") {

        volatile TaskControlBlock tcb0 = TASKCONTROLBLOCK_DEF(.priority = 0);
        volatile TaskControlBlock tcb1 = TASKCONTROLBLOCK_DEF(.priority = 1);
        volatile TaskControlBlock tcb2 = TASKCONTROLBLOCK_DEF(.priority = 2);
        volatile TaskControlBlock tcb3 = TASKCONTROLBLOCK_DEF(.priority = 1);

        volatile TaskControlBlock* head = &tcb2;
        tcb2.next = &tcb1;
        tcb1.next = &tcb0;

        _insert_tcb(&head, &tcb3);
        ASSERT_EQUAL_PTR(head, &tcb2, "head modified by insertion of duplicate TCB priority");
        ASSERT_EQUAL_PTR(head->next, &tcb1, "order modified by insertion of duplicate TCB priority");
        ASSERT_EQUAL_PTR(head->next->next, &tcb3, "duplicate TCB placement does not match FIFO rule");
        ASSERT_EQUAL_PTR(head->next->next->next, &tcb0, "order modified by insertion of duplicate TCB priority");
        ASSERT_EQUAL_PTR(head->next->next->next->next, NULL, "tail modified by insertion of duplicate TCB priority");
    
    } CASE_COMPLETE;

    TEST_CASE("insertion of NULL TCB") {

        volatile TaskControlBlock tcb0 = TASKCONTROLBLOCK_DEF();
        volatile TaskControlBlock* head = &tcb0;

        _insert_tcb(&head, NULL);
        ASSERT_EQUAL_PTR(head, &tcb0, "NULL TCB not assigned to head");
        ASSERT_EQUAL_INT(JOCKTOS_TCBError.invalid_tcb, 1, "invalid TCB error not logged");
        JOCKTOS_TCBError.invalid_tcb = 0;

    } CASE_COMPLETE;

    TEST_CASE("insertion of TCB to NULL list") {

        volatile TaskControlBlock** head_ref = NULL;
        volatile TaskControlBlock tcb = TASKCONTROLBLOCK_DEF();

        _insert_tcb(head_ref, &tcb);
        ASSERT_EQUAL_INT(JOCKTOS_TCBError.invalid_list_head, 1, "invalid list head error not logged");
        JOCKTOS_TCBError.invalid_list_head = 0;

    } CASE_COMPLETE;

    TEST_CASE("prevent duplicate TCB (loop/cyclic)") {

        unsigned int tcb_count = 0;
        int overflow_guard = 0;

        volatile TaskControlBlock* head = NULL;
        volatile TaskControlBlock tcb = TASKCONTROLBLOCK_DEF();

        _insert_tcb(&head, &tcb);
        _insert_tcb(&head, &tcb);

        while (head != NULL) {
            if (++overflow_guard > 100) {
                ASSERT_TRUE(false, "TCB cycle detected");
                break;
            }
            if (head == &tcb) tcb_count++;
            head = head->next;
        }

        ASSERT_EQUAL_INT(tcb_count, 1, "duplicate TCB inserted");
        ASSERT_EQUAL_INT(JOCKTOS_TCBError.invalid_tcb, 1, "duplicate TCB error not logged");
        JOCKTOS_TCBError.invalid_tcb = 0;

    } CASE_NOT_IMPLEMENTED;
}

void testRe_move_tcb() {

    TEST_CASE("removal of head of list") {

        volatile TaskControlBlock tcb0 = TASKCONTROLBLOCK_DEF(.priority = 0);
        volatile TaskControlBlock tcb1 = TASKCONTROLBLOCK_DEF(.priority = 1);
        volatile TaskControlBlock tcb2 = TASKCONTROLBLOCK_DEF(.priority = 2);

        volatile TaskControlBlock* head = &tcb2;
        tcb2.next = &tcb1;
        tcb1.next = &tcb0;

        _remove_tcb(&head, &tcb2);
        ASSERT_EQUAL_PTR(head, &tcb1, "TCB not removed from head");
        ASSERT_EQUAL_PTR(head->next, &tcb0, "TCB order not preserved");
        ASSERT_EQUAL_PTR(head->next->next, NULL, "TCB tail modified");

    } CASE_COMPLETE;

    TEST_CASE("removal of TCB in middle of list") {

        volatile TaskControlBlock tcb0 = TASKCONTROLBLOCK_DEF(.priority = 0);
        volatile TaskControlBlock tcb1 = TASKCONTROLBLOCK_DEF(.priority = 1);
        volatile TaskControlBlock tcb2 = TASKCONTROLBLOCK_DEF(.priority = 2);

        volatile TaskControlBlock* head = &tcb2;
        tcb2.next = &tcb1;
        tcb1.next = &tcb0;

        _remove_tcb(&head, &tcb1);
        ASSERT_EQUAL_PTR(head, &tcb2, "TCB not removed from head");
        ASSERT_EQUAL_PTR(head->next, &tcb0, "TCB order not preserved");
        ASSERT_EQUAL_PTR(head->next->next, NULL, "TCB tail modified");

    } CASE_COMPLETE;

    TEST_CASE("removal of tail of list") {
        
        volatile TaskControlBlock tcb0 = TASKCONTROLBLOCK_DEF(.priority = 0);
        volatile TaskControlBlock tcb1 = TASKCONTROLBLOCK_DEF(.priority = 1);
        volatile TaskControlBlock tcb2 = TASKCONTROLBLOCK_DEF(.priority = 2);

        volatile TaskControlBlock* head = &tcb2;
        tcb2.next = &tcb1;
        tcb1.next = &tcb0;

        _remove_tcb(&head, &tcb0);
        ASSERT_EQUAL_PTR(head, &tcb2, "head modified when tail removed");
        ASSERT_EQUAL_PTR(head->next, &tcb1, "TCB order not preserved");
        ASSERT_EQUAL_PTR(head->next->next, NULL, "TCB tail modified");

    } CASE_COMPLETE;

    TEST_CASE("removal of non-existent TCB") {

        volatile TaskControlBlock tcb0 = TASKCONTROLBLOCK_DEF(.priority = 0);
        volatile TaskControlBlock tcb1 = TASKCONTROLBLOCK_DEF(.priority = 1);
        volatile TaskControlBlock tcb2 = TASKCONTROLBLOCK_DEF(.priority = 2);

        volatile TaskControlBlock* head = &tcb2;
        tcb2.next = &tcb1;

        _remove_tcb(&head, &tcb0);
        ASSERT_EQUAL_PTR(head, &tcb2, "head modified when removing non-existent TCB");
        ASSERT_EQUAL_PTR(head->next, &tcb1, "TCB order not preserved");
        ASSERT_EQUAL_PTR(head->next->next, NULL, "TCB tail modified");

        ASSERT_EQUAL_INT(JOCKTOS_TCBError.invalid_list_element, 1, "invalid list element error not logged");
        JOCKTOS_TCBError.invalid_list_element = 0;

    } CASE_COMPLETE;

    TEST_CASE("removal of NULL TCB") {

        volatile TaskControlBlock tcb0 = TASKCONTROLBLOCK_DEF(.priority = 0);
        volatile TaskControlBlock tcb1 = TASKCONTROLBLOCK_DEF(.priority = 1);
        volatile TaskControlBlock tcb2 = TASKCONTROLBLOCK_DEF(.priority = 2);

        volatile TaskControlBlock* head = &tcb2;
        tcb2.next = &tcb1;
        tcb1.next = &tcb0;

        _remove_tcb(&head, NULL);
        ASSERT_EQUAL_PTR(head, &tcb2, "head modified when removing NULL TCB");
        ASSERT_EQUAL_PTR(head->next, &tcb1, "TCB order not preserved");
        ASSERT_EQUAL_PTR(head->next->next, &tcb0, "TCB order not preserved");
        ASSERT_EQUAL_PTR(head->next->next->next, NULL, "TCB tail modified when removing NULL TCB");

        ASSERT_EQUAL_INT(JOCKTOS_TCBError.invalid_tcb, 1, "invalid TCB error not logged");
        JOCKTOS_TCBError.invalid_tcb = 0;

    } CASE_COMPLETE;

    TEST_CASE("removal of TCB from NULL list") {

        volatile TaskControlBlock tcb0 = TASKCONTROLBLOCK_DEF(.priority = 0);
        volatile TaskControlBlock* head = NULL;

        _remove_tcb(&head, &tcb0);
        ASSERT_EQUAL_INT(JOCKTOS_TCBError.invalid_list_head, 1, "invalid list head error not logged");
        JOCKTOS_TCBError.invalid_list_head = 0;

    } CASE_COMPLETE;
}

void testUpdateTCB() {

    TEST_CASE("Modifying TCB priority") {

        volatile TaskControlBlock tcb0 = TASKCONTROLBLOCK_DEF(.priority = 0);
        volatile TaskControlBlock tcb1 = TASKCONTROLBLOCK_DEF(.priority = 1);

        volatile TaskControlBlock* head = &tcb1;
        tcb1.next = &tcb0;

        _update_tcb(&head, &tcb0, 2);
        ASSERT_EQUAL_INT(tcb0.priority, 2, "TCB priority not updated");
        ASSERT_EQUAL_PTR(head, &tcb0, "head modified when updating TCB priority");
        ASSERT_EQUAL_PTR(head->next, &tcb1, "TCB order not preserved");
        ASSERT_EQUAL_PTR(head->next->next, NULL, "TCB tail modified");

    } CASE_COMPLETE;

    TEST_CASE("Updating TCB in NULL headed list") {

        volatile TaskControlBlock* head = NULL;
        volatile TaskControlBlock tcb0 = TASKCONTROLBLOCK_DEF();

        _update_tcb(&head, &tcb0, 1);
        ASSERT_EQUAL_INT(JOCKTOS_TCBError.invalid_list_head, 1, "invalid list head error not logged");
        JOCKTOS_TCBError.invalid_list_head = 0;

        _update_tcb(NULL, &tcb0, 1);
        ASSERT_EQUAL_INT(JOCKTOS_TCBError.invalid_list_head, 1, "invalid list head error not logged");
        JOCKTOS_TCBError.invalid_list_head = 0;

    } CASE_COMPLETE;

    TEST_CASE("updating NULL TCB") {

        volatile TaskControlBlock tcb0 = TASKCONTROLBLOCK_DEF();
        volatile TaskControlBlock* head = &tcb0;

        _update_tcb(&head, NULL, 1);
        ASSERT_EQUAL_PTR(head, &tcb0, "head modified when updating NULL TCB");

        ASSERT_EQUAL_INT(JOCKTOS_TCBError.invalid_tcb, 1, "invalid TCB error not logged");
        JOCKTOS_TCBError.invalid_tcb = 0;

    } CASE_COMPLETE;
}

void testMoveTCB() {

    TEST_CASE("_move_tcb between valid lists") {

        volatile TaskControlBlock tcb0 = TASKCONTROLBLOCK_DEF(.priority = 0);
        volatile TaskControlBlock tcb1 = TASKCONTROLBLOCK_DEF(.priority = 1);
        volatile TaskControlBlock tcb2 = TASKCONTROLBLOCK_DEF(.priority = 2);
        volatile TaskControlBlock tcb3 = TASKCONTROLBLOCK_DEF(.priority = 3);

        volatile TaskControlBlock* head0 = &tcb1;
        volatile TaskControlBlock* head1 = &tcb3;

        tcb1.next = &tcb0;
        tcb3.next = &tcb2;

        _move_tcb(&head0, &tcb0, &head1);
        ASSERT_EQUAL_PTR(head0, &tcb1, "source list head modified when moving TCB");
        ASSERT_NOT_EQUAL_PTR(head0->next, &tcb0, "TCB not removed from source");

        ASSERT_EQUAL_PTR(head1, &tcb3, "destination list head modified when moving TCB");
        ASSERT_EQUAL_PTR(head1->next, &tcb2, "destination list order modified");
        ASSERT_EQUAL_PTR(head1->next->next, &tcb0, "TCB placed in wrong order");
        ASSERT_EQUAL_PTR(head1->next->next->next, NULL, "destination tail modified when moving TCB");

    } CASE_COMPLETE;
}

int main(void) {
    TEST_EVAL(testCreateTCB);
    TEST_EVAL(testInsertTCB);
    TEST_EVAL(testRe_move_tcb);
    TEST_EVAL(testUpdateTCB);
    TEST_EVAL(testMoveTCB);
    return testGetStatus();
}