#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "hashtable.h"

struct object {
    const char *key;
};

struct object objs[] = {
    {"hello world !"},
    {"The first step is as good as half over."},
    {"You never know your luck."},
    {"Sow nothing, reap nothing."},
    {"The wealth of the mind is the only wealth."},
    {"Variety is the spice of life."},
    {"Sharp tools make good work."},
    {NULL}
};

static int
test(struct hashtable *ht) {
    int i = 0;
    struct object *obj = &objs[i];
    while (obj->key != NULL) {
        assert(hashtable_put(ht, obj->key, (void *)obj) >= 0);
        fprintf(stdout, "put: %s\n", obj->key);
        obj = &objs[++i];
    }

    obj = (struct object *)hashtable_del(ht, "hello world !");
    assert(obj != NULL);
    fprintf(stdout, "del: %s\n", obj->key);

    return 1;
}

static void
delcb(void *ptr) {
    struct object *obj = (struct object *)ptr;
    fprintf(stdout, "release: %s\n", obj->key);
}

int
main(int argc, char *argv[]) {
    // use std malloc & free
    hashtable_global_init(NULL, NULL);

    struct hashtable *ht = hashtable_create(1361);
    assert(ht);

    assert(test(ht));

    hashtable_release(ht, delcb);

    return 0;
}