// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef LUNA_HASHMAP_H
#define LUNA_HASHMAP_H

#include "types.h"
#define MAP_SIZE 1024

typedef struct Node {
    char* key;
    union {
        LunaNumber number_value;
        LunaString string_value;
        LunaBoolean boolean_value;
    } as;
    LunaType value_type;
    struct Node* next;
} Node;

typedef struct HashMap {
    Node* map[MAP_SIZE];
} HashMap;

unsigned long hash(char* str);
Node* create_node(char* key, void* value, LunaType value_type);
void init_hash_map(HashMap* map);
void insert(HashMap* map, char* key, void* value, LunaType value_type);
Node* get_node(HashMap* map, char* key);
void remove_node(HashMap* map, char* key);
void free_hash_map(HashMap* map);

#endif