// Simple performance test of C hash table set

/*
C version (this code)
---------------------
$ gcc -Wall -O2 -o perfset-c samples/perfset.c ht.c
$ ./perfset-c samples/words.txt
setting 466550 keys: 0.120742000s
$ ./perfset-c samples/words.txt
setting 466550 keys: 0.117228000s
$ ./perfset-c samples/words.txt
setting 466550 keys: 0.119655000s
$ ./perfset-c samples/words.txt
setting 466550 keys: 0.119892000s
$ ./perfset-c samples/words.txt
setting 466550 keys: 0.123454000s
MINIMUM TIME: 0.117s (41% faster than Go)

Go version (see perfset.go)
---------------------------
$ go build -o perfset-go samples/perfset.go
$ ./perfset-go samples/words.txt
setting 466550 keys: 199.601477ms
$ ./perfset-go samples/words.txt
setting 466550 keys: 200.878177ms
$ ./perfset-go samples/words.txt
setting 466550 keys: 210.576502ms
$ ./perfset-go samples/words.txt
setting 466550 keys: 200.24748ms
$ ./perfset-go samples/words.txt
setting 466550 keys: 216.921344ms
MINIMUM TIME: 0.199s
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
