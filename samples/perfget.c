// Simple performance test of C hash table get

/*
gcc -O2 -o perfget-c samples/perfget.c ht.c

C version (this code):
----------------------
$ ./perfget-c samples/uniques.txt 
1000 runs getting 33435 keys: 0.923342000s
$ ./perfget-c samples/uniques.txt 
1000 runs getting 33435 keys: 0.942757000s
$ ./perfget-c samples/uniques.txt 
1000 runs getting 33435 keys: 0.902866000s
$ ./perfget-c samples/uniques.txt 
1000 runs getting 33435 keys: 0.928192000s
$ ./perfget-c samples/uniques.txt 
1000 runs getting 33435 keys: 0.913438000s
MINIMUM TIME: 0.903s (16% faster than Go)


go build -o perfget-go samples/perfget.go

Go version (see perfget.go):
----------------------------
$ ./perfget-go samples/uniques.txt 
1000 runs getting 33435 keys: 1.071121562s
$ ./perfget-go samples/uniques.txt 
1000 runs getting 33435 keys: 1.197651744s
$ ./perfget-go samples/uniques.txt 
1000 runs getting 33435 keys: 1.170963252s
$ ./perfget-go samples/uniques.txt 
1000 runs getting 33435 keys: 1.143383116s
$ ./perfget-go samples/uniques.txt 
1000 runs getting 33435 keys: 1.092975712s
MINIMUM TIME: 1.07s (18% slower than C)
*/

#include "../ht.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void exit_nomem(void) {
    fprintf(stderr, "out of memory\n");
    exit(1);
}

void* found;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: perftest file\n");
        return 1;
    }

    // Read entire file into memory.
    FILE* f = fopen(argv[1], "rb");
    if (f == NULL) {
        fprintf(stderr, "can't open file: %s\n", argv[1]);
        return 1;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* contents = (char*)malloc(size + 1);
    if (contents == NULL) {
        exit_nomem();
    }
    size_t nread = fread(contents, 1, size, f);
    if ((long)nread != size) {
        fprintf(stderr, "read %ld bytes instead of %ld", (long)nread, size);
        return 1;
    }
    fclose(f);
    contents[size] = 0;

    ht* counts = ht_create();
    if (counts == NULL) {
        exit_nomem();
    }

    for (char* p = contents; *p; p++) {
        // Skip whitespace.
        while (*p && *p <= ' ') {
            p++;
        }
        char* word = p;

        // Find end of word.
        while (*p && *p > ' ') {
            p++;
        }
        if (*p != 0) {
            *p = 0;
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

    // Copy keys to array
    char** keys = malloc(ht_length(counts) * sizeof(char*));
    if (keys == NULL) {
        exit_nomem();
    }
    hti it = ht_iterator(counts);
    int i = 0;
    while (ht_next(&it)) {
        keys[i] = it.key;
        i++;
    }

    int runs = 1000;
    clock_t start = clock();
    for (int run=0; run<runs; run++) {
        for (int i=0; i<ht_length(counts); i++) {
            found = ht_get(counts, keys[i]);
        }
    }
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    printf("%d runs getting %lu keys: %.09fs\n", runs, ht_length(counts), elapsed);

    return 0;
}
