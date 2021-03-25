// Simple hash table implemented in C.

#include "ht.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_SIZE 8

static ht* _ht_create(size_t size) {
    // Allocate space for hash table struct.
    ht* table = malloc(sizeof(ht));
    if (table == NULL) {
        return NULL;
    }
    table->_length = 0;
    table->_capacity = size * 2;

    // Allocate (zero'd) space for entry buckets.
    table->_entries = calloc(table->_capacity, sizeof(_ht_entry));
    if (table->_entries == NULL) {
        free(table); // error, free table before we return!
        return NULL;
    }
    return table;
}

ht* ht_create(void) {
    return _ht_create(INITIAL_SIZE);
}

void ht_destroy(ht* table) {
    // First free allocated keys.
    for (size_t i = 0; i < table->_capacity; i++) {
        if (table->_entries[i].key != NULL) {
            free(table->_entries[i].key);
        }
    }

    // Then free entries array and table itself.
    free(table->_entries);
    free(table);
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description at:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint64_t _hash(const char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

void* ht_get(ht* table, const char* key) {
    // AND hash with capacity-1 to ensure it's within entries array.
    uint64_t hash = _hash(key);
    size_t index = (size_t)(hash & (uint64_t)(table->_capacity - 1));

    // Loop till we find an empty entry.
    while (table->_entries[index].key != NULL) {
        if (strcmp(key, table->_entries[index].key) == 0) {
            // Found key, return value.
            return table->_entries[index].value;
        }
        // Key wasn't in this slot, move to next (linear probing).
        index++;
        if (index >= table->_capacity) {
            // At end of entries array, wrap around.
            index = 0;
        }
    }
    return NULL;
}

// Expand hash table to twice its current size. Return true on success,
// false if out of memory.
static bool _ht_expand(ht* table) {
    // Creating new table so we can use ht_set to move items into it.
    ht* new_table = _ht_create(table->_capacity);
    if (new_table == NULL) {
        return false;
    }

    // Iterate entries, move all non-empty ones to new table's entries.
    for (size_t i = 0; i < table->_capacity; i++) {
        _ht_entry entry = table->_entries[i];
        if (entry.key != NULL) {
            const char* key = ht_set(new_table, entry.key, entry.value);
            if (key == NULL) {
                ht_destroy(new_table);
                return false;
            }
        }
    }

    // Free old entries array and update this table's details.
    free(table->_entries);
    table->_entries = new_table->_entries;
    table->_capacity = new_table->_capacity;

    // Free new table structure (we've updated the existing one).
    free(new_table);
    return true;
}

const char* ht_set(ht* table, const char* key, void* value) {
    assert(value != NULL);
    if (value == NULL) {
        return NULL;
    }

    // AND hash with capacity-1 to ensure it's within entries array.
    uint64_t hash = _hash(key);
    size_t index = (size_t)(hash & (uint64_t)(table->_capacity - 1));

    // If length will exceed half of current capacity, expand it.
    if (table->_length >= table->_capacity / 2) {
        if (!_ht_expand(table)) {
            return NULL;
        }
        // Recalculate index as capacity has changed.
        index = (size_t)(hash & (uint64_t)(table->_capacity - 1));
    }

    // Loop till we find an empty entry.
    while (table->_entries[index].key != NULL) {
        if (strcmp(key, table->_entries[index].key) == 0) {
            // Found key (it already exists), update value.
            table->_entries[index].value = value;
            return table->_entries[index].key;
        }
        // Key wasn't in this slot, move to next (linear probing).
        index++;
        if (index >= table->_capacity) {
            // At end of entries array, wrap around.
            index = 0;
        }
    }

    // Didn't find key, allocate/copy key and insert it.
    char* key_copy = strdup(key);
    if (key_copy == NULL) {
        return NULL;
    }
    table->_entries[index].key = key_copy;
    table->_entries[index].value = value;
    table->_length++; // be sure to update length
    return key_copy;
}

size_t ht_length(ht* table) {
    return table->_length;
}

hti ht_iterator(ht* table) {
    hti it;
    it._table = table;
    it._index = 0;
    return it;
}

bool ht_next(hti* it) {
    // Loop till we've hit end of entries array.
    ht* table = it->_table;
    while (it->_index < table->_capacity) {
        size_t i = it->_index;
        it->_index++;
        if (table->_entries[i].key != NULL) {
            // Found next non-empty item, update iterator key and value.
            _ht_entry entry = table->_entries[i];
            it->key = entry.key;
            it->value = entry.value;
            return true;
        }
    }
    return false;
}
