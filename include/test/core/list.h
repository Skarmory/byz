#ifndef BYZ_TEST_CORE_LIST_H
#define BYZ_TEST_CORE_LIST_H

#include <stdbool.h>

bool test_list_free_data(void);
bool test_list_add(void);
bool test_list_rm(void);
bool test_list_find(void);
bool test_list_splice_node(void);
bool test_list_splice(void);
bool test_list_insert_after(void);
bool test_list_insert_before(void);
bool test_list_pop_head(void);
bool test_list_pop_at(void);
bool test_list_peek_head(void);
bool test_list_peek_tail(void);
bool test_list_empty(void);
bool test_list_sort(void);

void test_list_run_all_tests(void);

#endif
