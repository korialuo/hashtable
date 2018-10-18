#include "hashtable.h"
#include <stdlib.h>
#include <assert.h>

#define HASH_ID 0
#define HASH_A 1
#define HASH_B 2

struct bucket {
    uint32_t hash[2];
    void *val;
};

struct hashtable {
    int32_t len;
    int32_t size;
    struct bucket **slots;
};

static int g_init = 0;
static fn_alloc allocfn = 0;
static fn_free freefn = 0;

static uint32_t crypttable[0x500] = {0};

static uint32_t
hashfn(const char *key, int type) {
    uint8_t *k = (uint8_t *)key;
    uint32_t seed1 = 0x7fed7fed;
    uint32_t seed2 = 0xeeeeeeee;
    uint32_t ch;

    while (*k != 0) {
        ch = *k++;
        seed1 = crypttable[(type << 8) + ch] ^ (seed1 + seed2);
        seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
    }
    return seed1;
}

void
hashtable_global_init(fn_alloc al, fn_free fr) {
    if (!g_init) {
        g_init = 1;
        allocfn = (al ? al : malloc);
        freefn = (fr ? fr : free);

        uint32_t seed = 0x00100001, idx1, idx2, i;
        for (idx1 = 0; idx1 < 0x100; idx1++) {
            for (idx2 = idx1, i = 0; i < 5; i++, idx2 += 0x100) {
                uint32_t temp1, temp2;
                seed = (seed * 125 + 3) % 0x2AAAAB;
                temp1 = (seed & 0xFFFF) << 0x10;
                seed = (seed * 125 + 3) % 0x2AAAAB;
                temp2 = (seed & 0xFFFF);
                crypttable[idx2] = (temp1 | temp2);
            }
        }
    }
}

struct hashtable *
hashtable_create(int32_t size) {
    assert(size > 0);
    struct hashtable *ht = (struct hashtable *)allocfn(sizeof(*ht));
    ht->size = size;
    ht->len = 0;
    ht->slots = (struct bucket **)allocfn(ht->size * sizeof(void *));
    for (int i = 0; i < ht->size; ++i)
        ht->slots[i] = NULL;
    return ht;
}

int
hashtable_put(struct hashtable *ht, const char *key, void *val) {
    uint32_t hashid = hashfn(key, HASH_ID);
    struct bucket *bkt = allocfn(sizeof(*bkt));
    bkt->hash[0] = hashfn(key, HASH_A);
    bkt->hash[1] = hashfn(key, HASH_B);
    bkt->val = val;

    int32_t start = hashid % ht->size;
    int32_t pos = start;

    while (ht->slots[pos] != NULL) {
        pos = (pos + 1) % ht->size;
        if (pos == start) {
            freefn(bkt);
            return -1;
        }
    }
    ht->slots[pos] = bkt;
    ht->len++;
    return pos;
}

void *
hashtable_get(struct hashtable *ht, const char *key) {
    uint32_t hashid = hashfn(key, HASH_ID);
    uint32_t ha = hashfn(key, HASH_A);
    uint32_t hb = hashfn(key, HASH_B);

    int32_t start = hashid % ht->size;
    int32_t pos = start;

    struct bucket *bkt = NULL;
    while ((bkt = (struct bucket *)ht->slots[pos]) != NULL) {
        if (bkt->hash[0] == ha && bkt->hash[1] == hb) {
            return bkt->val;
        }
        else {
            pos = (pos + 1) % ht->size;
        }

        if (pos == start)
            break;
    }
    return NULL;
}

void *
hashtable_getidx(struct hashtable *ht, int32_t idx) {
    struct bucket *bkt = ht->slots[idx];
    if (bkt)
        return bkt->val;
    return NULL;
}

void *
hashtable_del(struct hashtable *ht, const char *key) {
    uint32_t hashid = hashfn(key, HASH_ID);
    uint32_t ha = hashfn(key, HASH_A);
    uint32_t hb = hashfn(key, HASH_B);

    int32_t start = hashid % ht->size;
    int32_t pos = start;

    struct bucket *bkt = NULL;
    void *val = NULL;
    while ((bkt = (struct bucket *)ht->slots[pos]) != NULL) {
        if (bkt->hash[0] == ha && bkt->hash[1] == hb) {
            val = bkt->val;
            ht->slots[pos] = NULL;
            freefn(bkt);
            ht->len--;
            break;
        }
        else {
            pos = (pos + 1) % ht->size;
        }

        if (pos == start)
            break;
    }
    return val;
}

int32_t
hashtable_len(struct hashtable *ht) {
    if (ht)
        return ht->len;
    return -1;
}

void
hashtable_release(struct hashtable *ht, cb_del delfn) {
    for (int32_t i = 0; i < ht->size; ++i) {
        struct bucket *bkt = ht->slots[i];
        if (bkt) {
            if (delfn)
                delfn(bkt->val);
            freefn(bkt);
        }
    }
    freefn(ht->slots);
    freefn(ht);
}
