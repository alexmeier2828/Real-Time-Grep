#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "line_list.h"
#include "test_utils.h"

void test_line_list_init() {
    line_list_t* list = line_list_init();
    
    test_assert(list != NULL, "line_list_init returns non-NULL");
    test_assert(list->length == 0, "line_list_init sets length to 0");
    test_assert(list->capacity == 500, "line_list_init sets capacity to 500");
    test_assert(list->items != NULL, "line_list_init allocates items array");
    
    line_list_deallocate(&list);
    test_assert(list == NULL, "line_list_deallocate sets pointer to NULL");
}

void test_line_list_add() {
    line_list_t* list = line_list_init();
    
    line_list_add(list, 5, "hello");
    test_assert(list->length == 1, "line_list_add increments length");
    test_assert(strcmp(list->items[0].contents, "hello") == 0, "line_list_add stores correct string");
    
    line_list_add(list, 5, "world");
    test_assert(list->length == 2, "line_list_add handles multiple additions");
    test_assert(strcmp(list->items[1].contents, "world") == 0, "line_list_add stores second string correctly");
    
    line_list_deallocate(&list);
}

void test_line_list_add_truncation() {
    line_list_t* list = line_list_init();
    
    line_list_add(list, 3, "hello");
    test_assert(strcmp(list->items[0].contents, "hel") == 0, "line_list_add truncates to specified length");
    
    line_list_deallocate(&list);
}

void test_line_list_clear() {
    line_list_t* list = line_list_init();
    
    line_list_add(list, 5, "hello");
    line_list_add(list, 5, "world");
    test_assert(list->length == 2, "setup: list has 2 items before clear");
    
    line_list_clear(list);
    test_assert(list->length == 0, "line_list_clear sets length to 0");
    
    line_list_deallocate(&list);
}

void test_line_list_capacity_expansion() {
    line_list_t* list = line_list_init();
    int initial_capacity = list->capacity;
    
    // Add enough items to trigger capacity expansion
    for (int i = 0; i < initial_capacity + 1; i++) {
        char buffer[20];
        sprintf(buffer, "item%d", i);
        line_list_add(list, strlen(buffer), buffer);
    }
    
    test_assert(list->length == initial_capacity + 1, "capacity expansion allows adding beyond initial capacity");
    test_assert(list->capacity > initial_capacity, "capacity expansion increases capacity");
    test_assert(strcmp(list->items[0].contents, "item0") == 0, "capacity expansion preserves first item");
    test_assert(strcmp(list->items[initial_capacity].contents, "item500") == 0, "capacity expansion preserves last item");
    
    line_list_deallocate(&list);
}

void test_line_list_empty_string() {
    line_list_t* list = line_list_init();
    
    line_list_add(list, 0, "");
    test_assert(list->length == 1, "line_list_add handles empty string");
    test_assert(strcmp(list->items[0].contents, "") == 0, "line_list_add stores empty string correctly");
    
    line_list_deallocate(&list);
}

void test_pane_initialization() {
    line_list_t* list = line_list_init();
    
    test_assert(list->pane != NULL, "pane is allocated on init");
    test_assert(list->pane->position == 0, "pane position starts at 0");
    test_assert(list->pane->length == 0, "pane length starts at 0");
    test_assert(list->pane->items == list->items, "pane items points to start of list");
    
    line_list_deallocate(&list);
}

void test_pane_after_adding_lines() {
    line_list_t* list = line_list_init();
    
    line_list_add(list, 5, "line1");
    test_assert(list->pane->length == 1, "pane length updates after adding line");
    test_assert(list->pane->items == list->items, "pane items still points to start");
    test_assert(strcmp(list->pane->items[0].contents, "line1") == 0, "pane can access added line");
    
    line_list_add(list, 5, "line2");
    line_list_add(list, 5, "line3");
    test_assert(list->pane->length == 3, "pane length reflects all lines");
    test_assert(strcmp(list->pane->items[2].contents, "line3") == 0, "pane can access last line");
    
    line_list_deallocate(&list);
}

void test_line_list_scroll_pane_basic() {
    line_list_t* list = line_list_init();
    
    line_list_add(list, 5, "line1");
    line_list_add(list, 5, "line2"); 
    line_list_add(list, 5, "line3");
    line_list_add(list, 5, "line4");
    
    test_assert(list->pane->position == 0, "initial pane position is 0");
    test_assert(list->pane->length == 4, "initial pane length is 4");
    
    line_list_scroll_pane(list, 1);
    test_assert(list->pane->position == 1, "scrolling by 1 updates position");
    test_assert(list->pane->length == 3, "scrolling by 1 updates length");
    test_assert(strcmp(list->pane->items[0].contents, "line2") == 0, "pane now starts at line2");
    
    line_list_scroll_pane(list, 2);
    test_assert(list->pane->position == 3, "scrolling by 2 more updates position");
    test_assert(list->pane->length == 1, "scrolling by 2 more updates length");
    test_assert(strcmp(list->pane->items[0].contents, "line4") == 0, "pane now starts at line4");
    
    line_list_deallocate(&list);
}

void test_line_list_scroll_pane_boundary() {
    line_list_t* list = line_list_init();
    
    line_list_add(list, 5, "line1");
    line_list_add(list, 5, "line2");
    
    line_list_scroll_pane(list, 5);
    test_assert(list->pane->position == 0, "scrolling beyond end does nothing");
    
    line_list_scroll_pane(list, 2);
    test_assert(list->pane->position == 0, "scrolling to exactly length does nothing");
    
    line_list_scroll_pane(list, 1);
    test_assert(list->pane->position == 1, "scrolling to length-1 works");
    
    line_list_scroll_pane(list, 1);
    test_assert(list->pane->position == 1, "scrolling beyond end from middle does nothing");
    
    line_list_deallocate(&list);
}

void test_line_list_scroll_pane_negative() {
    line_list_t* list = line_list_init();
    
    line_list_add(list, 5, "line1");
    line_list_add(list, 5, "line2");
    
    line_list_scroll_pane(list, -1);
    test_assert(list->pane->position == 0, "scrolling with n=-1 when position=0 keeps position at 0");
    test_assert(list->pane->length == 2, "scrolling with n=-1 when position=0 maintains correct length");
    
    line_list_deallocate(&list);
}

void test_line_list_scroll_pane_empty_list() {
    line_list_t* list = line_list_init();
    
    line_list_scroll_pane(list, 1);
    test_assert(list->pane->position == 0, "scrolling empty list does nothing");
    test_assert(list->pane->length == 0, "scrolling empty list keeps length 0");
    
    line_list_deallocate(&list);
}

void test_pane_after_clear() {
    line_list_t* list = line_list_init();
    
    line_list_add(list, 5, "line1");
    line_list_add(list, 5, "line2");
    line_list_scroll_pane(list, 1);
    
    test_assert(list->pane->position == 1, "setup: pane is scrolled");
    
    line_list_clear(list);
    test_assert(list->pane->position == 0, "clear resets pane position to 0");
    test_assert(list->pane->length == 0, "clear resets pane length to 0");
    test_assert(list->pane->items == list->items, "clear resets pane items pointer");
    
    line_list_deallocate(&list);
}

int run_line_list_tests() {
    reset_test_counters();
    printf("Running line_list tests...\n");
    
    test_line_list_init();
    test_line_list_add();
    test_line_list_add_truncation();
    test_line_list_clear();
    test_line_list_capacity_expansion();
    test_line_list_empty_string();
    test_pane_initialization();
    test_pane_after_adding_lines();
    test_line_list_scroll_pane_basic();
    test_line_list_scroll_pane_boundary();
    test_line_list_scroll_pane_negative();
    test_line_list_scroll_pane_empty_list();
    test_pane_after_clear();
    
    printf("\nLine list tests completed: %d/%d passed\n", test_passed, test_count);
    return (test_passed == test_count) ? 0 : 1;
}