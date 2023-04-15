// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

//
// Created by jr on 4/11/23.
//

#include "hash_map.h"
#include "repl.h"
#include <malloc.h>
#include <string.h>

unsigned long hash(char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++) != 0) { hash = ((hash << 5) + hash) + c; }

    return hash % MAP_SIZE;
}

Node* create_node(char* key, void* value, char type) {
    Node* node = (Node*) malloc(sizeof(Node));
    if (node == NULL) {
        print_error("Failed to initialize memory for hash map node\n");
        return NULL;
    }
    node->key = strdup(key);

    if (type == 0) {
        node->value_type = NVT_NUMBER;
        node->value_number = *(double*) value;
    } else if (type == 1) {
        node->value_type = NVT_STRING;
        node->value_str = (char*) value;
    } else {
        // Unsupported type
        return NULL;
    }

    node->next = NULL;
    return node;
}

void init_hash_map(HashMap* map) {
    for (int i = 0; i < MAP_SIZE; i++) { map->map[i] = NULL; }
}

void insert(HashMap* map, char* key, void* value, char type) {
    unsigned long index = hash(key);
    Node* node = create_node(key, value, type);
    node->next = map->map[index];
    map->map[index] = node;
}

// Get the value associated with a given key, or -1 if the key is not in the hash map
//void* get(HashMap* map, char* key) {
//    unsigned long index = hash(key);
//    Node* current = map->map[index];
//    while (current != NULL) {
//        if (strcmp(current->key, key) == 0) { return current->value.as_bytes; }
//        current = current->next;
//    }
//    return NULL;
//}

Node* get_node(HashMap* map, char* key) {
    unsigned long index = hash(key);
    Node* current = map->map[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) { return current; }
        current = current->next;
    }
    return NULL;
}

void remove_node(HashMap* map, char* key) {
    unsigned long index = hash(key);
    Node* current = map->map[index];
    Node* prev = NULL;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (prev == NULL) {
                map->map[index] = current->next;
            } else {
                prev->next = current->next;
            }

            free(current->key);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

void free_hash_map(HashMap* map) {
    for (int i = 0; i < MAP_SIZE; i++) {
        Node* current = map->map[i];
        while (current != NULL) {
            Node* next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
        map->map[i] = NULL;
    }
}
