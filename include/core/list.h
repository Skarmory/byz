#ifndef BYZ_LIST_H
#define BYZ_LIST_H

#include <stdbool.h>

typedef bool(*sort_func)(void* lhs, void* rhs);
typedef void(*dtor_func)(void* obj);

struct ListNode
{
    void* data;
    struct ListNode* next;
    struct ListNode* prev;
};

struct List
{
    int              count;
    struct ListNode* head;
    struct ListNode* tail;
};

struct List*     list_new(void);
void             list_free(struct List* list);
void             list_free_data(struct List* list, dtor_func dtor);
void             list_init(struct List* list);
void             list_uninit(struct List* list);
void             list_add(struct List* list, void* data);
void             list_add_head(struct List* list, void* data);
void             list_rm(struct List* list, struct ListNode* node);
struct ListNode* list_find(struct List* list, void* data);
void             list_splice_node(struct List* list_from, struct List* list_to, struct ListNode* node);
void             list_splice(struct List* list_from, struct List* list_to, int from_start, int to_start, int count);
void             list_insert_after(struct List* list, void* insert_this, struct ListNode* after_this);
void             list_insert_before(struct List* list, void* insert_this, struct ListNode* before_this);
void*            list_pop_head(struct List* list);
void*            list_pop_at(struct List* list, int index);
void*            list_peek_head(const struct List* list);
void*            list_peek_tail(const struct List* list);
bool             list_empty(const struct List* list);
void             list_sort(struct List* list, sort_func func);

#define list_for_each(list, it)\
    for(it = (list)->head; it != NULL; it = it->next)

#define list_for_each_safe(list, it, n)\
    for(it = (list)->head, n = (it ? it->next : NULL); it != NULL; it = n, n = (it ? it->next : NULL))

#endif
