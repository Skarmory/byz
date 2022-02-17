#include "core/list.h"

#include <stdlib.h>
#include <stdio.h>

static inline void _add_node(struct List* list, struct ListNode* node)
{
    if(list->tail)
    {
        node->prev = list->tail;
        list->tail->next = node;
    }
    else
    {
        list->head = node;
    }

    ++list->count;
    list->tail = node;
}

static inline void _remove_node(struct List* list, struct ListNode* node)
{
    if(node->prev)
    {
        node->prev->next = node->next;
    }
    else
    {
        list->head = node->next;
    }

    if(node->next)
    {
        node->next->prev = node->prev;
    }
    else
    {
        list->tail = node->prev;
    }

    node->prev = NULL;
    node->next = NULL;
    --list->count;
}

struct List* list_new(void)
{
    struct List* list = (struct List*)malloc(sizeof(struct List));

    list_init(list);

    return list;
}

void list_free(struct List* list)
{
    list_uninit(list);
    free(list);
}

void list_init(struct List* list)
{
    list->count = 0;
    list->head = NULL;
    list->tail = NULL;
}

void list_uninit(struct List* list)
{
    struct ListNode *node, *n;
    list_for_each_safe(list, node, n)
    {
        free(node);
    }

    list->count = 0;
    list->head = NULL;
    list->tail = NULL;
}

void list_add(struct List* list, void* data)
{
    struct ListNode* node = malloc(sizeof(struct ListNode));
    node->data = data;
    node->next = NULL;
    node->prev = NULL;

    _add_node(list, node);
}

void list_add_head(struct List* list, void* data)
{
    struct ListNode* node = malloc(sizeof(struct ListNode));
    node->data = data;
    node->next = NULL;
    node->prev = NULL;

    if(list->head)
    {
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }
    else
    {
        list->head = node;
        list->tail = node;
    }

    ++list->count;
}

void list_rm(struct List* list, struct ListNode* node)
{
    if(!node)
    {
        return;
    }

    _remove_node(list, node);

    free(node);
}

struct ListNode* list_find(struct List* list, void* data)
{
    struct ListNode* node;
    list_for_each(list, node)
    {
        if(node->data == data) return node;
    }

    return NULL;
}

void list_splice_node(struct List* list_from, struct List* list_to, struct ListNode* node)
{
    if(!node)
    {
        return;
    }

    _remove_node(list_from, node);

    _add_node(list_to, node);
}

void list_insert_after(struct List* list, void* insert_this, struct ListNode* after_this)
{
    struct ListNode* node = malloc(sizeof(struct ListNode));
    node->data = insert_this;
    node->next = after_this->next;
    node->prev = after_this;

    if(after_this->next)
    {
        after_this->next->prev = node;
    }
    else
    {
        list->tail = node;
    }

    after_this->next = node;

    ++list->count;
}
void list_insert_before(struct List* list, void* insert_this, struct ListNode* before_this)
{
    struct ListNode* node = malloc(sizeof(struct ListNode));
    node->data = insert_this;
    node->next = before_this;
    node->prev = before_this->prev;

    if(before_this->prev)
    {
        before_this->prev->next = node;
    }
    else
    {
        list->head = node;
    }

    before_this->prev = node;

    ++list->count;
}

void* list_pop_head(struct List* list)
{
    void* data = list->head->data;

    list_rm(list, list->head);

    return data;
}

void* list_peek_head(const struct List* list)
{
    return list->head->data;
}

void* list_peek_tail(const struct List* list)
{
    return list->tail->data;
}

bool list_empty(const struct List* list)
{
    return list->count == 0;
}

static void _sort(struct List* list, sort_func func)
{
    if(list->count <= 1)
    {
        return;
    }

    // Step 1: Split the top level list up into 2 sublists of equal(ish) size
    struct List left;
    struct List right;
    list_init(&left);
    list_init(&right);

    const int count = list->count;
    const int half_count = count/2;
    struct ListNode* node = list->head;
    struct ListNode* node_next = list->head->next;

    for(int i = 0; i < count; ++i)
    {
        if(i < half_count)
        {
            list_splice_node(list, &left, node);
        }
        else
        {
            list_splice_node(list, &right, node);
        }

        node = node_next;
        if(node_next)
        {
            node_next = node_next->next;
        }
    }

    // Step 2: Recurse the lists
    _sort(&left, func);
    _sort(&right, func);

    // Step 3: Compare the left and right list and splice them back onto the top level list
    struct ListNode* left_it = left.head;
    struct ListNode* right_it = right.head;
    while(left_it != NULL || right_it != NULL)
    {
        bool left_right = true;
        if(left_it && right_it)
        {
            // Both lists valid, so use compare function
            left_right = func(left_it->data, right_it->data);
        }
        else
        {
            // One of the lists is invalid, so check which is null to determine which to splice
            left_right = left_it != NULL;
        }

        if(left_right)
        {
            // Splict from left
            struct ListNode* n = left_it;
            left_it = left_it->next;
            list_splice_node(&left, list, n);
        }
        else
        {
            // Splice from right
            struct ListNode* n = right_it;
            right_it = right_it->next;
            list_splice_node(&right, list, n);
        }
    }
}

void list_sort(struct List* list, sort_func func)
{
    if(list->count == 0 || list->count == 1)
    {
        return;
    }

    _sort(list, func);
}
