// Print out some table buckets for use in the article

/*
$ gcc -Wall -O2 -o dump samples/dump.c ht.c && ./dump
index 0: empty
index 1: key jane, value 100
index 2: empty
index 3: empty
index 4: key bob, value 11
index 5: empty
index 6: empty
index 7: key foo, value 10
index 8: key bazz, value 36
index 9: key x, value 200
index 10: key bar, value 42
index 11: empty
index 12: key buzz, value 7
index 13: empty
index 14: empty
index 15: empty
*/

#include "../ht.h"

#include <stdio.h>
#include <stdlib.h>

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

typedef struct {
    char* key;
    int value;
} item;

void exit_nomem(void) {
    fprintf(stderr, "out of memory\n");
    exit(1);
}

int main(void) {
    item items[] = {
        {"foo", 10}, {"bar", 42}, {"bazz", 36}, {"buzz", 7},
        {"bob", 11}, {"jane", 100}, {"x", 200}};
    size_t num_items = sizeof(items) / sizeof(item);

    ht* table = ht_create();
    if (table == NULL) {
        exit_nomem();
    }

    for (int i = 0; i < num_items; i++) {
        if (ht_set(table, items[i].key, &items[i].value) == NULL) {
            exit_nomem();
        }
    }

    for (int i = 0; i < table->capacity; i++) {
        if (table->entries[i].key != NULL) {
            printf("index %d: key %s, value %d\n",
                i, table->entries[i].key, *(int*)table->entries[i].value);
        } else {
            printf("index %d: empty\n", i);
        }
    }

    return 0;
}
