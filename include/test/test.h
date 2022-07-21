#ifndef BYZ_TEST_TEST_H
#define ByZ_TEST_TEST_H

#include <stdbool.h>

#define TEST_CASE_FAIL false
#define TEST_CASE_SUCCESS true

bool test_assert_equal_char_buffer(const char* lhs, const char* rhs);
bool test_assert_equal_int(const int lhs, const int rhs);

#endif
