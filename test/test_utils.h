#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdio.h>

extern int test_count;
extern int test_passed;

void test_assert(int condition, const char* test_name);
void reset_test_counters(void);

#endif