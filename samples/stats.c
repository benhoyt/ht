// Show statistics of hash table

/*

$ python3 samples/gensimilar.py 466550 >samples/similar.txt
$ gcc -O2 -Wall -o stats samples/stats.c ht.c
$ ./stats <samples/words.txt
len=466550 cap=1048576 avgprobe=1.402
$ ./stats <samples/similar.txt
len=466550 cap=1048576 avgprobe=1.378

*/

#include "../ht.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void exit_nomem(void) {
    fprintf(stderr, "out of memory\n");
    exit(1);
}

// Copied from ht.c
typedef struct {
    char* key;  // key is NULL if this slot is empty
    void* value;
} ht_entry;

struct ht {
    ht_entry* entries;  // hash slots
    size_t capacity;    // size of _entries array
    size_t length;      // number of items in hash table
};

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

static uint64_t hash_key(const char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

// Copied from ht_get, but return probe length instead of value.
size_t get_probe_len(ht* table, const char* key) {
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(table->capacity - 1));

    size_t probe_len = 0;
    while (table->entries[index].key != NULL) {
        probe_len++;
        if (strcmp(key, table->entries[index].key) == 0) {
            return probe_len;
        }
        index++;
        if (index >= table->capacity) {
            index = 0;
        }
    }
    return probe_len;
}

int main(void) {
    ht* counts = ht_create();
    if (counts == NULL) {
        exit_nomem();
    }

    // Read next word from stdin (at most 100 chars long).
    char word[101];
    while (scanf("%100s", word) != EOF) {
        // Look up word.
        void* value = ht_get(counts, word);
        if (value != NULL) {
            // Already exists, increment int that value points to.
            int* pcount = (int*)value;
            (*pcount)++;
            continue;
        }

        // Word not found, allocate space for new int and set to 1.
        int* pcount = malloc(sizeof(int));
        if (pcount == NULL) {
            exit_nomem();
        }
        *pcount = 1;
        if (ht_set(counts, word, pcount) == NULL) {
            exit_nomem();
        }
    }

    // Calculate average probe length.
    hti it = ht_iterator(counts);
    size_t total_probes = 0;
    while (ht_next(&it)) {
        total_probes += get_probe_len(counts, it.key);
        free(it.value);
    }

    printf("len=%lu cap=%lu avgprobe=%.3f\n",
        ht_length(counts), counts->capacity, (double)total_probes / ht_length(counts));

    ht_destroy(counts);
    return 0;
}
