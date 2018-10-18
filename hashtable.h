// Blizzard Hashtable (MIT License)
// Author: korialuo (https://github.com/korialuo/hashtable)
// Create: 2018/10/18

#ifndef __hashtable_h__
#define __hashtable_h__

#include <stdint.h>
#include <stddef.h>

typedef void *(*fn_alloc)(size_t size);
typedef void (*fn_free)(void *ptr);
typedef void (*cb_del)(void *ptr);

// global init
void hashtable_global_init(fn_alloc al, fn_free fr);

// create a hashtable
// suggested size: 1361, 2729, 5471, 10949, 21911, 87719, 175447, 350899 ...
struct hashtable *hashtable_create(int32_t size);

// put an object into hashtable
// return -1 for failed, >= 0 for index.
int32_t hashtable_put(struct hashtable *ht, const char *key, void *val);

// get an object by key
void *hashtable_get(struct hashtable *ht, const char *key);

// get an object by index
void *hashtable_getidx(struct hashtable *ht, int32_t idx);

// remove an object by key
// return the object if exists, or return NULL.
void *hashtable_del(struct hashtable *ht, const char *key);

// get length of the hashtable
// return -1 for error.
int32_t hashtable_len(struct hashtable *ht);

// destroy a hashtable
// del callback for release every object in the hashtable. NULL for no need.
void hashtable_release(struct hashtable *ht, cb_del delfn);

#endif // __hashtable_h__