#include "test_utils.h"

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

void reset_test_counters(void) {
    test_count = 0;
    test_passed = 0;
}