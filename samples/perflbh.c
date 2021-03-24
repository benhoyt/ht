// Performance comparison of lookups: linear search, binary search, hash table

#include "../ht.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void exit_nomem(void) {
    fprintf(stderr, "out of memory\n");
    exit(1);
}

typedef struct {
    char* key;
    int value;
} item;

item* linear_search(item* items, size_t size, const char* key) {
    for (size_t i=0; i<size; i++) {
        if (strcmp(items[i].key, key) == 0) {
            return &items[i];
        }
    }
    return NULL;
}

item* binary_search(item* items, size_t size, const char* key) {
    size_t low = 0;
    size_t high = size;

    while (low < high) {
        size_t mid = (low+high) / 2;
        int c = strcmp(items[mid].key, key);
        if (c == 0) {
            return &items[mid];
        }
        if (c < 0) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }
    if (low < size && strcmp(items[low].key, key) == 0) {
        return &items[low];
    }
    return NULL;
}

item items[] = {
    {"aoo", 0},
    {"bar", 1},
    {"cazz", 2},
    {"duzz", 3},
    {"eaddle", 4},
    {"fche", 5},
    {"garf", 6},
    {"hache", 7},
    {"iya", 8},
    {"jay", 9},
    {"kay", 10},
    {"lell", 11},
    {"memm", 12},
    {"noo", 0},
    {"oar", 1},
    {"pazz", 2},
    {"quzz", 3},
    {"raddle", 4},
    {"sche", 5},
    {"tarf", 6},
    {"uache", 7},
    {"vya", 8},
    {"way", 9},
    {"xay", 10},
    {"yell", 11},
    {"zemm", 12},
};

item* found;
int* value;

int main(int argc, char **argv) {
    int runs = 5000000;

    for (int num_items = 1; num_items < sizeof(items)/sizeof(item); num_items++) {
        printf("NUM ITEMS: %d\n", num_items);

        clock_t start = clock();
        for (int item_index=0; item_index<num_items; item_index++) {
            char* key = items[item_index].key;
            for (int run=0; run<runs; run++) {
                found = linear_search(items, num_items, key);
            }
        }
        clock_t end = clock();
        double elapsed = (double)(end - start) / CLOCKS_PER_SEC / num_items;
        printf("  linear, %d runs: %.09fs\n", runs, elapsed);

        start = clock();
        for (int item_index=0; item_index<num_items; item_index++) {
            char* key = items[item_index].key;
            for (int run=0; run<runs; run++) {
                found = binary_search(items, num_items, key);
            }
        }
        end = clock();
        elapsed = (double)(end - start) / CLOCKS_PER_SEC / num_items;
        printf("  binary, %d runs: %.09fs\n", runs, elapsed);

        ht* table = ht_create();
        if (table == NULL) {
            exit_nomem();
        }
        for (int item_index=0; item_index<num_items; item_index++) {
            int* pn = malloc(sizeof(int));
            if (pn == NULL) {
                exit_nomem();
            }
            *pn = items[item_index].value;
            const char* p = ht_set(table, items[item_index].key, pn);
            if (p == NULL) {
                exit_nomem();
            }
        }
        start = clock();
        for (int item_index=0; item_index<num_items; item_index++) {
            char* key = items[item_index].key;
            for (int run=0; run<runs; run++) {
                value = (int*)ht_get(table, key);
            }
        }
        end = clock();
        elapsed = (double)(end - start) / CLOCKS_PER_SEC / num_items;
        printf("  hash  , %d runs: %.09fs\n", runs, elapsed);
    }
}
