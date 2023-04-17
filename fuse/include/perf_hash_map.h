// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef FUSION_PERF_HASH_MAP_H
#define FUSION_PERF_HASH_MAP_H

#define TABLE_SIZE 4096

#include "repl.h"
#include "types.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

typedef struct FunctionObject {
    char* name;
    int argc;
    char** args_names;
    FusionType return_type;
    int return_address;
    int entry_point;
} FunctionObject;

typedef struct HashNode {
    char* key;
    FunctionObject value;
    int is_occupied;
} HashNode;

typedef struct PHashMap {
    HashNode* nodes;
    int size;
    int count;
} PHashMap;

unsigned long hash_function(const char* str);

unsigned long hash_function_2(const char* str);

PHashMap* perf_hash_map_new();

void perf_hash_map_free(PHashMap* map);

void perf_hash_map_insert(PHashMap* map, const char* key, FunctionObject value);

FunctionObject* perf_hash_map_get(PHashMap* map, const char* key);

#endif  //FUSION_PERF_HASH_MAP_H
