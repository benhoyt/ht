// Examples of binary search (with and without bsearch)

/*

$ gcc -O2 -o bsearch samples/bsearch.c && ./bsearch 
bsearch: value of 'bob' is 11
binary_search: value of 'bob' is 11

*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char* key;
    int value;
} item;

int cmp(const void* a, const void* b) {
    item* item_a = (item*)a;
    item* item_b = (item*)b;
    return strcmp(item_a->key, item_b->key);
}

item* binary_search(item* items, size_t size, const char* key) {
    if (size + size < size) {
        return NULL; // size too big; avoid overflow
    }
    size_t low = 0;
    size_t high = size;
    while (low < high) {
        size_t mid = (low + high) / 2;
        int c = strcmp(items[mid].key, key);
        if (c == 0) {
            return &items[mid];
        }
        if (c < 0) {
            low = mid + 1; // eliminate low half of array
        } else {
            high = mid;    // eliminate high half of array
        }
    }
    // Entire array has been eliminated, key not found.
    return NULL;
}

int main(void) {
    item items[] = {
        {"bar", 42}, {"bazz", 36}, {"bob", 11}, {"buzz", 7},
        {"foo", 10}, {"jane", 100}, {"x", 200}};
    size_t num_items = sizeof(items) / sizeof(item);

    item key = {"bob", 0};
    item* found = bsearch(&key, items, num_items, sizeof(item), cmp);
    if (found == NULL) {
        return 1;
    }
    printf("bsearch: value of 'bob' is %d\n", found->value);

    found = binary_search(items, num_items, "bob");
    if (found == NULL) {
        return 1;
    }
    printf("binary_search: value of 'bob' is %d\n", found->value);
    return 0;
}
