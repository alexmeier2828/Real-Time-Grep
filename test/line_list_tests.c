#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../line_list.h"

int test_count = 0;
int test_passed = 0;

void test_assert(int condition, const char* test_name) {
    test_count++;
    if (condition) {
        printf("PASS: %s\n", test_name);
        test_passed++;
    } else {
        printf("FAIL: %s\n", test_name);
    }
}

void test_line_list_init() {
    line_list_t* list = line_list_init();
    
    test_assert(list != NULL, "line_list_init returns non-NULL");
    test_assert(list->length == 0, "line_list_init sets length to 0");
    test_assert(list->capacity == 500, "line_list_init sets capacity to 500");
    test_assert(list->lines != NULL, "line_list_init allocates lines array");
    
    line_list_deallocate(&list);
    test_assert(list == NULL, "line_list_deallocate sets pointer to NULL");
}

void test_line_list_add() {
    line_list_t* list = line_list_init();
    
    line_list_add(list, 5, "hello");
    test_assert(list->length == 1, "line_list_add increments length");
    test_assert(strcmp(list->lines[0], "hello") == 0, "line_list_add stores correct string");
    
    line_list_add(list, 5, "world");
    test_assert(list->length == 2, "line_list_add handles multiple additions");
    test_assert(strcmp(list->lines[1], "world") == 0, "line_list_add stores second string correctly");
    
    line_list_deallocate(&list);
}

void test_line_list_add_truncation() {
    line_list_t* list = line_list_init();
    
    line_list_add(list, 3, "hello");
    test_assert(strcmp(list->lines[0], "hel") == 0, "line_list_add truncates to specified length");
    
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
        char buffer[10];
        sprintf(buffer, "item%d", i);
        line_list_add(list, strlen(buffer), buffer);
    }
    
    test_assert(list->length == initial_capacity + 1, "capacity expansion allows adding beyond initial capacity");
    test_assert(list->capacity > initial_capacity, "capacity expansion increases capacity");
    test_assert(strcmp(list->lines[0], "item0") == 0, "capacity expansion preserves first item");
    test_assert(strcmp(list->lines[initial_capacity], "item500") == 0, "capacity expansion preserves last item");
    
    line_list_deallocate(&list);
}

void test_line_list_empty_string() {
    line_list_t* list = line_list_init();
    
    line_list_add(list, 0, "");
    test_assert(list->length == 1, "line_list_add handles empty string");
    test_assert(strcmp(list->lines[0], "") == 0, "line_list_add stores empty string correctly");
    
    line_list_deallocate(&list);
}

int run_line_list_tests() {
    printf("Running line_list tests...\n");
    
    test_line_list_init();
    test_line_list_add();
    test_line_list_add_truncation();
    test_line_list_clear();
    test_line_list_capacity_expansion();
    test_line_list_empty_string();
    
    printf("\nLine list tests completed: %d/%d passed\n", test_passed, test_count);
    return (test_passed == test_count) ? 0 : 1;
}