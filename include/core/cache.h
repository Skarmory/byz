#ifndef BYZ_CACHE_H
#define BYZ_CACHE_H

/* Stores items in contiguous memory, with a free list, and generational handles.
 *
 * Handles embed the generation key and the storage index.
 */

struct cache;

typedef void(*free_function)(void*);

struct cache* cache_new(int item_size, int capacity, free_function free_func);
void          cache_free(struct cache* cache);

int           cache_size(struct cache* cache);
int           cache_used(struct cache* cache);

int           cache_add(struct cache* cache, void* item);
void          cache_remove(struct cache* cache, int handle);
void*         cache_get(struct cache* cache, int handle);

#endif
