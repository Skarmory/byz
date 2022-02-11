#include "core/cache.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

const int NULL_CACHE_HANDLE = 0xffffffff;

static const int cs_idx_mask     = 0b00000000000000001111111111111111; // <<  0
static const int cs_key_mask     = 0b01111111111111110000000000000000; // << 16
static const int cs_invalid_mask = 0b10000000000000000000000000000000; // << 31
static const int cs_valid_mask   = 0b01111111111111111111111111111111;

static const int cs_key_shift = 16;

static const int cs_max_generations = cs_key_mask >> cs_key_shift;
static const int cs_max_capacity = cs_idx_mask - 1;

struct cache
{
    void* items;
    int*  handles;
    int   current_used;
    int   max_used;
    int   item_size;
    int   capacity;
    int   free_head;
    free_function free_func;
};

// Bit logic to make our key and index into a handle
static int _make_handle(int key, int idx)
{
    // 0x00kkkkkkiiiiiiii
    return ( key << cs_key_shift ) | idx;
}

// Bit logic to get the item array index from the handle
static int _get_idx(int handle)
{
    return handle & cs_idx_mask;
}

// Bit logic to get the key generation from the handle
static int _get_key(int handle)
{
    int key = handle & cs_valid_mask;
    key &= cs_key_mask;
    key >>= cs_key_shift;
    return key;
}

// Return the bytes offset from the beginning of cache items array
static int _get_item_offset(int item_size, int handle)
{
    return item_size * _get_idx(handle);
}

// Returns pointer to the item in the cache items array
static void* _get_item(struct cache* cache, int handle)
{
    return cache->items + _get_item_offset(cache->item_size, handle);
}

// Check if this handle "points at" a null index
// This signifies that this handle is invalid and possibly the end of the free list
static bool _points_to_null(int handle)
{
    return (_get_idx(handle) & cs_idx_mask) == cs_idx_mask;
}

/* Get the next handle index, either from the free list or open up a new entry.
 * If retrieving from the free list, make sure the free list is maintained.
 * Return invalid handle if there is nothing available.
 */
static int _next_handle(struct cache* cache)
{
    int handle = NULL_CACHE_HANDLE;
    int key = cs_key_mask;
    int idx = cs_idx_mask;

    if(!_points_to_null(cache->free_head))
    {
        // Get from free list
        idx = _get_idx(cache->free_head);
        cache->handles[idx] &= cs_valid_mask;

        key = _get_key(cache->handles[idx]);
        int next_idx = _get_idx(cache->handles[idx]);

        ++key;
        if(key == cs_max_generations)
        {
            printf("WARNING: KEY ROLLOVER AT INDEX: %d\n", idx);
            key = 0;
        }

        // Make return handle
        handle = _make_handle(key, idx);

        // Make entry valid
        cache->handles[idx] = handle;

        // Set free head to next node along
        cache->free_head = _make_handle(0, next_idx);
        if(next_idx == cs_idx_mask)
        {
            cache->free_head |= cs_invalid_mask;
        }
    }
    else
    {
        if(cache->max_used == cache->capacity)
        {
            return handle;
        }

        // No free list, open up new entry
        key = 0;
        idx = cache->max_used;

        cache->handles[idx] = _make_handle(key, idx);
        cache->handles[idx] &= cs_valid_mask;

        handle = cache->handles[idx];

        ++cache->max_used;
    }

    return handle;
}

// Check if the handle is marked "invalid", therefore is a free slot
static bool _check_valid(int handle)
{
    return (handle & cs_invalid_mask) == 0;
}

// Checks whether the handle passed in by other code matches the handle the cache is expecting
static bool _check_handle(struct cache* cache, int handle)
{
    int idx = _get_idx(handle);

    if(idx >= cache->max_used)
    {
        return false;
    }

    if(!_check_valid(cache->handles[idx]))
    {
        return false;
    }

    if(cache_stale_handle(cache, handle))
    {
        return false;
    }

    return true;
}

#if DEBUG
static void debug_print_free_list(struct cache* cache)
{
    int idx = _get_idx(cache->free_head);

    do
    {
        printf("%d > ", idx);

        idx = _get_idx(cache->handles[idx]);
    }
    while(idx != cs_idx_mask);

    printf("%d\n ", cs_idx_mask);
}

static void debug_print_handle(int handle)
{
    if(!_check_valid(handle))
    {
        printf("*");
    }

    printf("(i:%d, k:%d)", _get_idx(handle), _get_key(handle));
}

static void debug_print_handles(struct cache* cache)
{
    debug_print_handle(cache->handles[0]);

    for(int i = 1; i < cache->max_used; ++i)
    {
        printf(", ");
        debug_print_handle(cache->handles[i]);
    }

    printf("\n");
}
#endif

struct cache* cache_new(int item_size, int capacity, free_function free_func)
{
    assert(capacity <= cs_max_capacity);

    struct cache* cache = calloc(1, sizeof(struct cache));

    cache->items     = calloc(capacity, item_size);
    cache->handles   = malloc(sizeof(int) * capacity);
    cache->item_size = item_size;
    cache->capacity  = capacity;
    cache->free_head = NULL_CACHE_HANDLE;
    cache->free_func = free_func;

    memset(cache->handles, NULL_CACHE_HANDLE, sizeof(int) * capacity);

    return cache;
}

void cache_free(struct cache* cache)
{
    if(cache->free_func)
    {
        for(int i = 0; i < cache->max_used; ++i)
        {
            int handle = cache->handles[i];
            if(_check_valid(handle))
            {
                cache->free_func(_get_item(cache, handle));
            }
        }
    }

    free(cache->items);
    free(cache->handles);
    free(cache);
}

int cache_size(struct cache* cache)
{
    return cache->current_used;
}

int cache_capacity(struct cache* cache)
{
    return cache->capacity;
}

int cache_item_size(struct cache* cache)
{
    return cache->item_size;
}

int cache_used(struct cache* cache)
{
    return cache->max_used;
}

// Check if the key generation for a handle matches what the cache is holding
// If false, the slot has been freed and reused
bool cache_stale_handle(struct cache* cache, int handle)
{
    return _get_key(handle) != _get_key(cache->handles[_get_idx(handle)]);
}

int cache_add(struct cache* cache, void* item)
{
    int next_handle = _next_handle(cache);

    if(_check_valid(next_handle))
    {
        memcpy(_get_item(cache, next_handle), item, cache->item_size);
        ++cache->current_used;
    }

    return next_handle;
}

void cache_remove(struct cache* cache, int handle)
{
    if(!_check_handle(cache, handle))
    {
        return;
    }

    int handle_idx = _get_idx(handle);

    if(cache->free_func)
    {
        cache->free_func(_get_item(cache, handle));
    }

    if(!_points_to_null(cache->free_head))
    {
        // Set invalidated handle to point at what free head is pointing at
        cache->handles[handle_idx] = _make_handle(_get_key(cache->handles[handle_idx]), _get_idx(cache->free_head));
        cache->handles[handle_idx] |= cs_invalid_mask;

        // Set free head to point at invalidated handle
        cache->free_head = _make_handle(0, handle_idx);
    }
    else
    {
        // Set invalidated handle to point at index mask
        cache->handles[handle_idx] = _make_handle(_get_key(cache->handles[handle_idx]), cs_idx_mask);
        cache->handles[handle_idx] |= cs_invalid_mask;

        // Set free head to point at invalidated handle
        cache->free_head = _make_handle(0, handle_idx);
    }

    --cache->current_used;
}

void* cache_get(struct cache* cache, int handle)
{
    if(!_check_handle(cache, handle))
    {
        return NULL;
    }

    return _get_item(cache, handle);
}
