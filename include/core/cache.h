#ifndef BYZ_CACHE_H
#define BYZ_CACHE_H

#include <stdbool.h>

/* Stores items in contiguous memory, with a free list, and generational handles.
 *
 * Handles embed the generation key and the storage index.
 */

struct CACHE;

typedef void(*free_function)(void*);

extern const int NULL_CACHE_HANDLE;

struct CACHE* cache_new(int item_size, int capacity, free_function free_func);
void          cache_free(struct CACHE* cache);

int           cache_size(struct CACHE* cache);
int           cache_capacity(struct CACHE* cache);
int           cache_item_size(struct CACHE* cache);
int           cache_used(struct CACHE* cache);
bool          cache_stale_handle(struct CACHE* cache, int handle);

int           cache_add(struct CACHE* cache, void* item);
void          cache_remove(struct CACHE* cache, int handle);
void*         cache_get(struct CACHE* cache, int handle);

#endif
