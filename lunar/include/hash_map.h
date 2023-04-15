// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef LUNA_HASHMAP_H
#define LUNA_HASHMAP_H

#define MAP_SIZE 1024

typedef enum {
    NVT_NUMBER,
    NVT_STRING,
} NodeValueType;

typedef struct Node {
    char* key;
    double value_number;
    char* value_str;
    NodeValueType value_type;
    struct Node* next;
} Node;

typedef struct HashMap {
    Node* map[MAP_SIZE];
} HashMap;

unsigned long hash(char* str);
Node* create_node(char* key, void* value, char type);
void init_hash_map(HashMap* map);
void insert(HashMap* map, char* key, void* value, char type);
//void* get(HashMap* map, char* key);
Node* get_node(HashMap* map, char* key);
void remove_node(HashMap* map, char* key);
void free_hash_map(HashMap* map);

#endif