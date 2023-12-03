// Copyright © 2023 Jake Rieger.
//
// Licensed under the GNU General Public License v3.0

#ifndef FUSION_HASHMAP_H
#define FUSION_HASHMAP_H

#include "types.h"
#include <malloc.h>
#include <stdbool.h>

#define INITIAL_CAPACITY 16
#define DEFAULT_LOAD_FACTOR 0.75f

typedef enum MapValueType {
    MAP_TYPE_NULL,
    MAP_TYPE_BOOLEAN,
    MAP_TYPE_NUMBER,
    MAP_TYPE_STRING,
    MAP_TYPE_FUNCTION,
} MapValueType;

typedef struct Entry {
    char* key;
    union {
        void* null_value;
        FusionBoolean bool_value;
        FusionNumber number_value;
        FusionString string_value;
        FunctionObject function_value;
    } as;
    MapValueType value_type;
} Entry;

typedef struct HashMap {
    Entry* entries;
    int capacity;
    int size;
    float load_factor;
} HashMap;

static unsigned int hash_string(const char* str);

static bool is_prime(int n);

static int next_prime(int n);

HashMap* hashmap_create();

void hashmap_destroy(HashMap* map);

static void rehash(HashMap* map);

void hashmap_put(HashMap* map, const char* key, void* value, MapValueType value_type);

bool hashmap_contains_key(const HashMap* map, const char* key);

Entry* hashmap_get(const HashMap* map, const char* key);

bool hashmap_remove(HashMap* map, const char* key);

int hashmap_size(const HashMap* map);

bool hashmap_is_empty(const HashMap* map);

void print_hashmap(HashMap* map);

#endif