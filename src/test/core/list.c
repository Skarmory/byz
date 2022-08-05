#include "test/core/list.h"

#include "test/test.h"

#include "core/list.h"

#include <stddef.h>
#include <stdlib.h>

static int s_dtor_called = 0;

struct ListTest
{
    int* iblock1;
    int* iblock2;
    struct List l1;
    struct List l2;
};

static struct ListTest init_test(void)
{
    struct ListTest t;
    t.iblock1 = malloc(sizeof(int) * 10);
    t.iblock2 = malloc(sizeof(int) * 10);
    list_init(&t.l1);
    list_init(&t.l2);

    for(int i = 0; i < 10; ++i)
    {
        t.iblock1[i] = i;
        t.iblock2[i] = i;
        list_add(&t.l1, &t.iblock1[i]);
        list_add(&t.l2, &t.iblock2[i]);
    }

    return t;
}

static void uninit_test(struct ListTest* t)
{
    free(t->iblock1);
    free(t->iblock2);
    list_uninit(&t->l1);
    list_uninit(&t->l2);
}

static void _dtor(void* data)
{
    (void)data;
    ++s_dtor_called;
}

static bool _sort(void* lhs, void* rhs)
{
    return *(int*)lhs <= *(int*)rhs;
}

static bool _test_list_values(struct List* l, const int* expect, const int expect_len)
{
    bool success = true;
    int i = 0;
    struct ListNode* n = NULL;
    list_for_each(l, n)
    {
        success &= test_assert_equal_int(expect[i++], *(int*)n->data);
    }

    success &= test_assert_equal_int(expect_len, i);

    return success;
}

static bool _test_list_free_data_dtor(void)
{
    bool success = true;

    s_dtor_called = 0;

    struct List l;
    list_init(&l);

    for(int i = 0; i < 10; ++i)
    {
        int* ip = malloc(sizeof(int));
        list_add(&l, ip);
    }

    success &= test_assert_equal_int(l.count, 10);

    list_free_data(&l, &_dtor);

    success &= test_assert_equal_int(l.count, 0);
    success &= test_assert_equal_int(s_dtor_called, 10);

    list_uninit(&l);

    return success;
}

static bool _test_list_free_data_no_dtor(void)
{
    bool success = true;

    struct List l;
    list_init(&l);

    for(int i = 0; i < 10; ++i)
    {
        int* ip = malloc(sizeof(int));
        list_add(&l, ip);
    }

    success &= test_assert_equal_int(l.count, 10);

    list_free_data(&l, NULL);

    success &= test_assert_equal_int(l.count, 0);

    list_uninit(&l);
    return success;
}

bool test_list_free_data(void)
{
    bool success = true;
    success &= test_run_test("free data with dtor func", &_test_list_free_data_dtor);
    success &= test_run_test("free data no dtor func", &_test_list_free_data_no_dtor);
    return success;
}

bool _test_list_add_one(void)
{
    bool success = true;

    struct List l;
    list_init(&l);

    int* i = malloc(sizeof(int));
    *i = 77;

    list_add(&l, i);

    success &= test_assert_equal_int(l.count, 1);
    success &= test_assert_not_null(l.head);
    success &= test_assert_not_null(l.tail);
    success &= test_assert_equal_int(77, *(int*)l.head->data);
    success &= test_assert_equal_int(77, *(int*)l.tail->data);
    success &= test_assert_null(l.head->next);
    success &= test_assert_null(l.head->prev);
    success &= test_assert_null(l.tail->next);
    success &= test_assert_null(l.tail->prev);

    free(i);
    list_uninit(&l);

    return success;
}

static bool _test_list_add_many(void)
{
    bool success = true;

    int* iblock = malloc(100 * sizeof(int));
    struct List l;
    list_init(&l);

    for(int i = 0; i < 100; ++i)
    {
        int* ip = iblock + i;
        *ip = i;
        list_add(&l, ip);
    }

    success &= _test_list_values(&l, iblock, 100);
    success &= test_assert_equal_int(l.count, 100);
    success &= test_assert_not_null(l.head);
    success &= test_assert_not_null(l.tail);
    success &= test_assert_null(l.head->prev);
    success &= test_assert_null(l.tail->next);
    success &= test_assert_not_null(l.head->next);
    success &= test_assert_not_null(l.tail->prev);

    list_uninit(&l);
    free(iblock);

    return success;
}

static bool _test_list_add_head(void)
{
    bool success = true;

    int* i1 = malloc(sizeof(int));
    int* i2 = malloc(sizeof(int));
    int* i3 = malloc(sizeof(int));

    *i1 = 0;
    *i2 = 1;
    *i3 = 2;

    struct List l;
    list_init(&l);

    list_add(&l, i1);
    list_add(&l, i2);

    success &= test_assert_equal_int(0, *(int*)(l.head->data));
    success &= test_assert_equal_int(1, *(int*)(l.head->next->data));

    list_add_head(&l, i3);

    success &= test_assert_equal_int(2, *(int*)(l.head->data));
    success &= test_assert_equal_int(0, *(int*)(l.head->next->data));

    list_uninit(&l);
    free(i1);
    free(i2);
    free(i3);

    return success;
}

bool test_list_add(void)
{
    bool success = true;

    success &= test_run_test("add one item", &_test_list_add_one);
    success &= test_run_test("add many items", &_test_list_add_many);
    success &= test_run_test("add at head", &_test_list_add_head);

    return success;
}

static bool _test_list_rm(void)
{
    bool success = true;

    int* i1 = malloc(sizeof(int));
    int* i2 = malloc(sizeof(int));
    int* i3 = malloc(sizeof(int));

    *i1 = 0;
    *i2 = 1;
    *i3 = 2;

    struct List l;
    list_init(&l);

    list_add(&l, i1);
    list_add(&l, i2);
    list_add(&l, i3);

    success &= test_assert_equal_int(*(int*)(l.head->next->data), 1);

    list_rm(&l, l.head->next);

    success &= test_assert_equal_int(l.count, 2);
    success &= test_assert_equal_int(*(int*)(l.head->next->data), 2);

    list_uninit(&l);
    free(i1);
    free(i2);
    free(i3);

    return success;
}

bool test_list_remove(void)
{
    bool success = true;
    success &= test_run_test("remove node", &_test_list_rm);
    return success;
}

static bool _test_list_find_in_list(void)
{
    bool success = true;

    struct ListTest lt = init_test();

    int* can_find = lt.l1.head->next->data;

    struct ListNode* ln = list_find(&lt.l1, can_find);
    success &= test_assert_not_null(ln);

    uninit_test(&lt);

    return success;
}

static bool _test_list_find_not_in_list(void)
{
    bool success = true;

    struct ListTest lt = init_test();

    int cant_find = 77;

    struct ListNode* ln = list_find(&lt.l1, &cant_find);
    success &= test_assert_null(ln);

    uninit_test(&lt);

    return success;
}

bool test_list_find(void)
{
    bool success = true;
    success &= test_run_test("find is in list", &_test_list_find_in_list);
    success &= test_run_test("find is not in list", &_test_list_find_not_in_list);
    return success;
}

bool _test_list_splice_node_to_empty_list(void)
{
    bool success = true;

    struct ListTest lt = init_test();

    struct List empty_list;
    list_init(&empty_list);

    success &= test_assert_equal_int(10, lt.l1.count);
    success &= test_assert_equal_int(0, empty_list.count);
    success &= test_assert_equal_int(0, *(int*)lt.l1.head->data);
    success &= test_assert_null(empty_list.head);
    success &= test_assert_null(empty_list.tail);

    list_splice_node(&lt.l1, &empty_list, lt.l1.head);

    success &= test_assert_equal_int(9, lt.l1.count);
    success &= test_assert_equal_int(1, empty_list.count);
    success &= test_assert_equal_int(1, *(int*)lt.l1.head->data);
    success &= test_assert_equal_int(0, *(int*)empty_list.head->data);
    success &= test_assert_not_null(lt.l1.head);
    success &= test_assert_not_null(lt.l1.tail);
    success &= test_assert_not_null(empty_list.head);
    success &= test_assert_not_null(empty_list.tail);

    uninit_test(&lt);
    list_uninit(&empty_list);

    return success;
}

bool _test_list_splice_node_to_non_empty_list(void)
{
    bool success = true;

    struct ListTest lt = init_test();

    success &= test_assert_equal_int(10, lt.l1.count);
    success &= test_assert_equal_int(10, lt.l2.count);
    success &= test_assert_equal_int(9, *(int*)lt.l2.tail->data);
    success &= test_assert_equal_int(0, *(int*)lt.l1.head->data);

    list_splice_node(&lt.l1, &lt.l2, lt.l1.head);

    success &= test_assert_equal_int(9, lt.l1.count);
    success &= test_assert_equal_int(11, lt.l2.count);
    success &= test_assert_equal_int(1, *(int*)lt.l1.head->data);
    success &= test_assert_equal_int(0, *(int*)lt.l2.tail->data);
    success &= test_assert_not_null(lt.l1.head);
    success &= test_assert_not_null(lt.l1.tail);
    success &= test_assert_not_null(lt.l2.head);
    success &= test_assert_not_null(lt.l2.tail);

    uninit_test(&lt);

    return success;
}

bool _test_list_splice_range_to_empty_list(void)
{
    bool success = true;

    struct ListTest lt = init_test();
    struct List el;
    list_init(&el);

    success &= test_assert_equal_int(10, lt.l1.count);
    success &= test_assert_equal_int(0, el.count);

    list_splice(&lt.l1, &el, 3, 0, 5);

    success &= test_assert_equal_int(5, lt.l1.count);
    success &= test_assert_equal_int(5, el.count);
    success &= test_assert_null(el.head->prev);
    success &= test_assert_null(el.tail->next);

    const int expected_l1[] = { 0, 1, 2, 8, 9 };
    success &= _test_list_values(&lt.l1, expected_l1, 5);

    const int expected_l2[] = { 3, 4, 5, 6, 7 };
    success &= _test_list_values(&el, expected_l2, 5);

    list_uninit(&el);
    uninit_test(&lt);

    return success;
}

bool _test_list_splice_range_to_middle_of_list(void)
{
    bool success = true;

    struct ListTest lt = init_test();

    success &= test_assert_equal_int(10, lt.l1.count);
    success &= test_assert_equal_int(10, lt.l2.count);

    list_splice(&lt.l1, &lt.l2, 3, 7, 5);

    success &= test_assert_equal_int(5, lt.l1.count);
    success &= test_assert_equal_int(15, lt.l2.count);
    success &= test_assert_null(lt.l2.head->prev);
    success &= test_assert_null(lt.l2.tail->next);

    const int expected_l1[] = { 0, 1, 2, 8, 9 };
    success &= _test_list_values(&lt.l1, expected_l1, 5);

    const int expected_l2[] = { 0, 1, 2, 3, 4, 5, 6, 7, 3, 4, 5, 6, 7, 8, 9 };
    success &= _test_list_values(&lt.l2, expected_l2, 15);

    uninit_test(&lt);

    return success;
}

bool test_list_splicing(void)
{
    bool success = true;

    success &= test_run_test("splice node to empty list", &_test_list_splice_node_to_empty_list);
    success &= test_run_test("splice node to not empty list", &_test_list_splice_node_to_non_empty_list);
    success &= test_run_test("splice range to empty list", &_test_list_splice_range_to_empty_list);
    success &= test_run_test("splice range to not empty list", &_test_list_splice_range_to_middle_of_list);

    return success;
}

static bool _test_list_insert_after(void)
{
    bool success = true;

    int test_value = 77777;

    struct ListTest lt = init_test();

    list_insert_after(&lt.l1, &test_value, lt.l1.head->next);

    const int expected[] = { 0, 1, test_value, 2, 3, 4, 5, 6, 7, 8, 9 };
    success &= _test_list_values(&lt.l1, expected, 11);
    success &= test_assert_equal_int(11, lt.l1.count);

    uninit_test(&lt);

    return success;
}

static bool _test_list_insert_before(void)
{
    bool success = true;

    int test_value = 77777;

    struct ListTest lt = init_test();

    list_insert_before(&lt.l1, &test_value, lt.l1.tail->prev);

    const int expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, test_value, 8, 9 };
    success &= _test_list_values(&lt.l1, expected, 11);
    success &= test_assert_equal_int(11, lt.l1.count);

    uninit_test(&lt);

    return success;
}

bool test_list_insert(void)
{
    bool success = true;
    success &= test_run_test("insert after", &_test_list_insert_after);
    success &= test_run_test("insert before", &_test_list_insert_before);
    return success;
}

bool _test_list_pop_head(void)
{
    bool success = true;

    struct ListTest lt = init_test();

    success &= test_assert_equal_int(0, *(int*)lt.l1.head->data);
    success &= test_assert_equal_int(10, lt.l1.count);

    int* data = list_pop_head(&lt.l1);

    success &= test_assert_equal_int(1, *(int*)lt.l1.head->data);
    success &= test_assert_equal_int(0, *data);
    success &= test_assert_equal_int(9, lt.l1.count);

    uninit_test(&lt);

    return success;
}

bool _test_list_pop_at(void)
{
    bool success = true;

    struct ListTest lt = init_test();

    success &= test_assert_equal_int(4, *(int*)lt.l1.head->next->next->next->next->data);
    success &= test_assert_equal_int(10, lt.l1.count);

    int* data = list_pop_at(&lt.l1, 4);

    success &= test_assert_equal_int(5, *(int*)lt.l1.head->next->next->next->next->data);
    success &= test_assert_equal_int(4, *data);
    success &= test_assert_equal_int(9, lt.l1.count);

    uninit_test(&lt);

    return success;
}

bool test_list_pop(void)
{
    bool success = true;

    success &= test_run_test("pop head", &_test_list_pop_head);
    success &= test_run_test("pop at", &_test_list_pop_at);

    return success;
}


bool _test_list_peek_head(void)
{
    bool success = true;

    struct ListTest lt = init_test();

    int* data = list_peek_head(&lt.l1);

    success &= test_assert_equal_int(0, *data);
    success &= test_assert_equal_int(10, lt.l1.count);

    uninit_test(&lt);

    return success;
}

static bool _test_list_peek_tail(void)
{
    bool success = true;

    struct ListTest lt = init_test();

    int* data = list_peek_tail(&lt.l1);

    success &= test_assert_equal_int(9, *data);
    success &= test_assert_equal_int(10, lt.l1.count);

    uninit_test(&lt);

    return success;
}

bool test_list_peek(void)
{
    bool success = true;

    success &= test_run_test("peek head", &_test_list_peek_head);
    success &= test_run_test("peek tail", &_test_list_peek_tail);

    return success;
}

static bool _test_list_empty_empty_list(void)
{
    bool success = true;

    struct List l;
    list_init(&l);

    success &= test_assert_equal_bool(true, list_empty(&l));
    success &= test_assert_equal_int(0, l.count);

    list_uninit(&l);

    return success;
}
static bool _test_list_empty_not_empty_list(void)
{
    bool success = true;

    struct ListTest lt = init_test();

    success &= test_assert_equal_bool(false, list_empty(&lt.l1));
    success &= test_assert_equal_int(10, lt.l1.count);

    uninit_test(&lt);

    return success;
}

bool test_list_empty(void)
{
    bool success = true;

    success &= test_run_test("list empty with empty list", &_test_list_empty_empty_list);
    success &= test_run_test("list empty with not empty list", &_test_list_empty_not_empty_list);

    return success;
}

static bool _test_list_sort(void)
{
    bool success = true;
    struct List l;
    list_init(&l);

    int* iblock = malloc(sizeof(int) * 10);

    for(int i = 0; i < 10; ++i)
    {
        iblock[9 - i] =  i;
        list_add(&l, &iblock[9 - i]);
    }

    list_sort(&l, &_sort);

    const int expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    success &= _test_list_values(&l, expected, 10);

    list_uninit(&l);
    free(iblock);
    return success;
}

bool test_list_sort(void)
{
    bool success = true;
    success &= test_run_test("sort", &_test_list_sort);
    return success;
}

void test_list_run_all_tests(void)
{
    test_run_test_block("list free data", &test_list_free_data);
    test_run_test_block("list add", &test_list_add);
    test_run_test_block("list remove", &test_list_remove);
    test_run_test_block("list find", &test_list_find);
    test_run_test_block("list splicing", &test_list_splicing);
    test_run_test_block("list insert", &test_list_insert);
    test_run_test_block("list pop", &test_list_pop);
    test_run_test_block("list peek", &test_list_peek);
    test_run_test_block("list empty", &test_list_empty);
    test_run_test_block("list sort", &test_list_sort);
}
