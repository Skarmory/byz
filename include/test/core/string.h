#ifndef BYZ_TEST_CORE_STRING_H
#define BYZ_TEST_CORE_STRING_H

#include <stdbool.h>

struct string;

bool test_string_new(void);
bool test_string_init(void);
bool test_string_size(void);

void test_string_run_all_tests(void);

#endif
