// Simple performance test of C hash table set

/*
C version (this code)
---------------------
$ python3 samples/genuniques.py 1000000 > samples/uniques.txt
$ gcc -O2 -o perfset-c samples/perfset.c ht.c
$ ./perfset-c samples/uniques.txt
setting 1000000 keys: 0.675686000s
$ ./perfset-c samples/uniques.txt
setting 1000000 keys: 0.707720000s
$ ./perfset-c samples/uniques.txt
setting 1000000 keys: 0.626419000s
$ ./perfset-c samples/uniques.txt
setting 1000000 keys: 0.613833000s
$ ./perfset-c samples/uniques.txt
setting 1000000 keys: 0.636187000s
MINIMUM TIME: 0.613s (23% slower than Go)

Go version (see perfset.go)
---------------------------
go build -o perfset-go samples/perfset.go
$ ./perfset-go samples/uniques.txt
setting 1000000 keys: 503.734977ms
$ ./perfset-go samples/uniques.txt
setting 1000000 keys: 509.6396ms
$ ./perfset-go samples/uniques.txt
setting 1000000 keys: 504.439856ms
$ ./perfset-go samples/uniques.txt
setting 1000000 keys: 498.694129ms
$ ./perfset-go samples/uniques.txt
setting 1000000 keys: 503.554704ms
MINIMUM TIME: 0.499s
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

    ht* table = ht_create();
    if (table == NULL) {
        exit_nomem();
    }

    int value = 1; // dummy value
    clock_t start = clock();
    for (int i=0; i<ht_length(counts); i++) {
        if (ht_set(table, keys[i], &value) == NULL) {
            exit_nomem();
        }
    }
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    printf("setting %lu keys: %.09fs\n", ht_length(counts), elapsed);

    return 0;
}
