#ifndef BYZ_TEST_TEST_H
#define ByZ_TEST_TEST_H

#include <stdbool.h>

#define TEST_CASE_FAIL false
#define TEST_CASE_SUCCESS true

typedef bool(*test_func)(void);

bool test_assert_equal_char_buffer(const char* expect, const char* actual);
bool test_assert_equal_int(const int expect, const int actual);

void test_run_test_block(const char* block_name, test_func func);
bool test_run_test(const char* test_name, test_func func);

#endif
