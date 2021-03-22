// Simple hash table implementation in C

#include "../ht.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char* word;
    int count;
} count;

// Comparison function for qsort that orders by count descending.
int cmp_count(const void* p1, const void* p2) {
    int c1 = ((count*)p1)->count;
    int c2 = ((count*)p2)->count;
    if (c1 == c2) return 0;
    if (c1 < c2) return 1;
    return -1;
}

void exit_nomem(void) {
    fprintf(stderr, "out of memory\n");
    exit(1);
}

int main(void) {
    ht* counts = ht_create();
    if (counts == NULL) {
        exit_nomem();
    }

    // Read next word from stdin (at most 100 chars long).
    char word[101];
    while (scanf("%100s", word) != EOF) {
        // Convert word to lowercase in place.
        for (char* p = word; *p; p++) {
            *p = tolower(*p);
        }

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

    // Allocate space for array of word-count pairs.
    count* ordered = calloc(ht_length(counts), sizeof(count));
    if (ordered == NULL) {
        exit_nomem();
    }

    // Iterate items and copy to array (freeing allocated ints as we go).
    hti it = ht_iterator(counts);
    for (size_t i = 0; ht_next(&it); i++) {
        ordered[i].word = it.key;
        ordered[i].count = *(int*)it.value;
        free(it.value);
    }

    // Call qsort to sort by count, most frequent first.
    qsort(ordered, ht_length(counts), sizeof(count), cmp_count);

    // Print "word count" pairs in that order.
    for (size_t i = 0; i < ht_length(counts); i++) {
        printf("%s %d\n", ordered[i].word, ordered[i].count);
    }

    // Free hash table.
    ht_destroy(counts);
    return 0;
}
