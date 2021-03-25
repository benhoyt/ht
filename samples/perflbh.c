// Performance comparison of lookups: linear search, binary search, hash table

/*

$ gcc -O2 -Wall -o perflbh samples/perflbh.c ht.c
$ ./perflbh 
NUM ITEMS: 1
  linear, 5000000 runs: 0.046263000s
  binary, 5000000 runs: 0.038028000s
  hash  , 5000000 runs: 0.055019000s
NUM ITEMS: 2
  linear, 5000000 runs: 0.029790500s
  binary, 5000000 runs: 0.042562000s
  hash  , 5000000 runs: 0.051395500s
NUM ITEMS: 3
  linear, 5000000 runs: 0.034900667s
  binary, 5000000 runs: 0.045250667s
  hash  , 5000000 runs: 0.050218000s
NUM ITEMS: 4
  linear, 5000000 runs: 0.042225750s
  binary, 5000000 runs: 0.053457000s
  hash  , 5000000 runs: 0.049959500s
NUM ITEMS: 5
  linear, 5000000 runs: 0.048300800s
  binary, 5000000 runs: 0.054513400s
  hash  , 5000000 runs: 0.056074000s
NUM ITEMS: 6
  linear, 5000000 runs: 0.056408000s
  binary, 5000000 runs: 0.062688833s
  hash  , 5000000 runs: 0.059385833s
NUM ITEMS: 7
  linear, 5000000 runs: 0.062235571s  # this is where linear starts getting slower
  binary, 5000000 runs: 0.059203714s
  hash  , 5000000 runs: 0.058669571s
NUM ITEMS: 8
  linear, 5000000 runs: 0.074767000s
  binary, 5000000 runs: 0.063110250s
  hash  , 5000000 runs: 0.054628375s
NUM ITEMS: 9
  linear, 5000000 runs: 0.086145111s
  binary, 5000000 runs: 0.066840778s
  hash  , 5000000 runs: 0.057300111s
NUM ITEMS: 10
  linear, 5000000 runs: 0.085599600s
  binary, 5000000 runs: 0.069974600s
  hash  , 5000000 runs: 0.052592700s
...

*/

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
