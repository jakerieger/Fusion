// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#include "perf_hash_map.h"

unsigned long hash_function(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }

    return hash;
}

unsigned long hash_function_2(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) { hash = ((hash << 5) + hash) + c; }

    return 1 + (hash % (TABLE_SIZE - 1));
}

PHashMap* perf_hash_map_new() {
    PHashMap* map = (PHashMap*) malloc(sizeof(PHashMap));
    map->nodes = (HashNode*) calloc(TABLE_SIZE, sizeof(HashNode));
    map->size = TABLE_SIZE;
    map->count = 0;
    return map;
}

void perf_hash_map_free(PHashMap* map) {
    int i;
    for (i = 0; i < map->size; i++) {
        if (map->nodes[i].is_occupied) { free(map->nodes[i].key); }
    }
    free(map->nodes);
    free(map);
}

void perf_hash_map_insert(PHashMap* map, const char* key, FunctionObject value) {
    if (map->count == map->size) {
        print_error("Hash map is full\n");
        exit(1);
    }

    unsigned long index = hash_function(key) % map->size;
    unsigned step = hash_function_2(key) % (map->size - 1);

    while (map->nodes[index].is_occupied) {
        if (strcmp(map->nodes[index].key, key) == 0) {
            map->nodes[index].value = value;
            exit(1);
        }

        index = (index + step) % map->size;
    }

    map->nodes[index].key = strdup(key);
    map->nodes[index].value = value;
    map->nodes[index].is_occupied = 1;
    map->count++;
}

FunctionObject* perf_hash_map_get(PHashMap* map, const char* key) {
    unsigned long index = hash_function(key) % map->size;
    unsigned long step = hash_function_2(key) % (map->size - 1);

    while (map->nodes[index].is_occupied) {
        if (strcmp(map->nodes[index].key, key) == 0) { return &(map->nodes[index].value); }

        index = (index + step) % map->size;
    }

    return NULL;
}