#include "hashmap.h"
#include "repl.h"
#include "types.h"
#include <string.h>
#include <strings.h>

static unsigned int hash_string(const char* str) {
    unsigned int hash = 5381;
    int c;

    while ((c = *str++)) { hash = ((hash << 5) + hash) + c; }

    return hash;
}

static bool is_prime(int n) {
    if (n < 2) { return false; }

    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) { return false; }
    }

    return true;
}

static int next_prime(int n) {
    while (!is_prime(n)) { n++; }

    return n;
}

HashMap* hashmap_create() {
    HashMap* map = (HashMap*) malloc(sizeof(HashMap));
    map->capacity = INITIAL_CAPACITY;
    map->size = 0;
    map->load_factor = DEFAULT_LOAD_FACTOR;
    map->entries = (Entry*) calloc(map->capacity, sizeof(Entry));
    return map;
}

void hashmap_destroy(HashMap* map) {
    for (int i = 0; i < map->capacity; i++) {
        if (map->entries[i].key != NULL) { free(map->entries[i].key); }
    }

    free(map->entries);
    free(map);
}

// TODO: Fix implementation to support new value type
static void rehash(HashMap* map) {
    int old_capacity = map->capacity;
    Entry* old_entries = map->entries;

    map->capacity = next_prime(old_capacity * 2);
    map->entries = (Entry*) calloc(map->capacity, sizeof(Entry));
    map->size = 0;

    for (int i = 0; i < old_capacity; i++) {
        if (old_entries[i].key != NULL) {
            // hashmap_put(map, old_entries[i].key, old_entries[i].value);
            switch (old_entries[i].value_type) {
                case MAP_TYPE_BOOLEAN: {
                    FusionBoolean* value = malloc(sizeof(FusionBoolean));
                    memcpy(&old_entries[i].as.bool_value, value, sizeof(FusionBoolean));
                    hashmap_put(map, old_entries[i].key, value, MAP_TYPE_BOOLEAN);
                } break;
                case MAP_TYPE_NUMBER: {
                    FusionNumber* value = malloc(sizeof(FusionNumber));
                    memcpy(&old_entries[i].as.number_value, value, sizeof(FusionNumber));
                    hashmap_put(map, old_entries[i].key, value, MAP_TYPE_BOOLEAN);
                } break;
                case MAP_TYPE_STRING: {
                    FusionString* value = malloc(sizeof(FusionString));
                    memcpy(&old_entries[i].as.string_value, value, sizeof(FusionString));
                    hashmap_put(map, old_entries[i].key, value, MAP_TYPE_STRING);
                } break;
                case MAP_TYPE_FUNCTION: {
                    FunctionObject* value = malloc(sizeof(FunctionObject));
                    memcpy(&old_entries[i].as.function_value, value, sizeof(FunctionObject));
                    hashmap_put(map, old_entries[i].key, value, MAP_TYPE_FUNCTION);
                } break;
                case MAP_TYPE_NULL:
                default: {
                    print_error("Tried to assign symbol 'null' value\n");
                    exit(1);
                }
            }
            free(old_entries[i].key);
        }
    }

    free(old_entries);
}

// TODO: Fix implementation to support new value type
void hashmap_put(HashMap* map, const char* key, void* value, MapValueType value_type) {
    if ((float) (map->size + 1) / map->capacity > map->load_factor) { rehash(map); }

    unsigned int hash = hash_string(key);
    int index = hash % map->capacity;

    while (map->entries[index].key != NULL) {
        if (strcmp(map->entries[index].key, key) == 0) {
            switch (value_type) {
                case MAP_TYPE_BOOLEAN: {
                    map->entries[index].value_type = value_type;
                    memcpy(&map->entries[index].as.bool_value, (FusionBoolean*) value,
                           sizeof(FusionBoolean));
                    //map->entries[index].as.bool_value = *(FusionBoolean*) value;
                } break;
                case MAP_TYPE_NUMBER: {
                    map->entries[index].value_type = value_type;
                    memcpy(&map->entries[index].as.number_value, (FusionNumber*) value,
                           sizeof(FusionNumber));
                    //map->entries[index].as.number_value = *(FusionNumber*) value;
                } break;
                case MAP_TYPE_STRING: {
                    map->entries[index].value_type = value_type;
                    map->entries[index].as.string_value = strdup((FusionString) value);
                    //map->entries[index].as.string_value = (FusionString) value;
                } break;
                case MAP_TYPE_FUNCTION: {
                    map->entries[index].value_type = value_type;
                    memcpy(&map->entries[index].as.function_value, (FunctionObject*) value,
                           sizeof(FunctionObject));
                    //map->entries[index].as.function_value = *(FunctionObject*) value;
                } break;
                case MAP_TYPE_NULL:
                default: {
                    print_error("Tried to assign symbol 'null' value\n");
                    exit(1);
                }
            }
            return;
        }

        index = (index + 1) % map->capacity;
    }

    map->entries[index].key = strdup(key);
    switch (value_type) {
        case MAP_TYPE_BOOLEAN: {
            map->entries[index].value_type = value_type;
            memcpy(&map->entries[index].as.bool_value, (FusionBoolean*) value,
                   sizeof(FusionBoolean));
            //map->entries[index].as.bool_value = *(FusionBoolean*) value;
        } break;
        case MAP_TYPE_NUMBER: {
            map->entries[index].value_type = value_type;
            memcpy(&map->entries[index].as.number_value, (FusionNumber*) value,
                   sizeof(FusionNumber));
            //map->entries[index].as.number_value = *(FusionNumber*) value;
        } break;
        case MAP_TYPE_STRING: {
            map->entries[index].value_type = value_type;
            map->entries[index].as.string_value = strdup((FusionString) value);
            //map->entries[index].as.string_value = (FusionString) value;
        } break;
        case MAP_TYPE_FUNCTION: {
            map->entries[index].value_type = value_type;
            memcpy(&map->entries[index].as.function_value, (FunctionObject*) value,
                   sizeof(FunctionObject));
            //map->entries[index].as.function_value = *(FunctionObject*) value;
        } break;
        case MAP_TYPE_NULL:
        default: {
            print_error("Tried to assign symbol 'null' value\n");
            exit(1);
        }
    }
    map->size++;
}

bool hashmap_contains_key(const HashMap* map, const char* key) {
    unsigned int hash = hash_string(key);
    int index = hash % map->capacity;

    while (map->entries[index].key != NULL) {
        if (strcmp(map->entries[index].key, key) == 0) { return true; }

        index = (index + 1) % map->capacity;
    }

    return false;
}

// TODO: Fix implementation to support new value type
Entry* hashmap_get(const HashMap* map, const char* key) {
    unsigned int hash = hash_string(key);
    int index = hash % map->capacity;

    while (map->entries[index].key != NULL) {
        if (strcmp(map->entries[index].key, key) == 0) { return &map->entries[index]; }

        index = (index + 1) % map->capacity;
    }

    return 0;
}

bool hashmap_remove(HashMap* map, const char* key) {
    unsigned int hash = hash_string(key);
    int index = hash % map->capacity;

    while (map->entries[index].key != NULL) {
        if (strcmp(map->entries[index].key, key) == 0) {
            free(map->entries[index].key);
            map->entries[index].key = NULL;
            map->entries[index].value_type = MAP_TYPE_NULL;
            map->entries[index].as.null_value = NULL;
            map->entries[index].as.number_value = 0x0;
            map->entries[index].as.string_value = NULL;
            bzero(&map->entries[index].as.bool_value, sizeof(FusionBoolean));
            bzero(&map->entries[index].as.function_value, sizeof(FunctionObject));
            map->size--;
            return true;
        }

        index = (index + 1) % map->capacity;
    }

    return false;
}

int hashmap_size(const HashMap* map) { return map->size; }

bool hashmap_is_empty(const HashMap* map) { return map->size == 0; }

void print_hashmap(HashMap* map) {
    printf("{\n");

    for (int i = 0; i < map->capacity; i++) {
        if (map->entries[i].key != NULL) {
            switch (map->entries[i].value_type) {
                case MAP_TYPE_BOOLEAN: {
                    printf("  '%s': %s,\n", map->entries[i].key,
                           map->entries[i].as.bool_value.value == 0x0 ? "true" : "false");
                } break;
                case MAP_TYPE_NUMBER: {
                    printf("  '%s': %f,\n", map->entries[i].key, map->entries[i].as.number_value);
                } break;
                case MAP_TYPE_STRING: {
                    printf("  '%s': \"%s\",\n", map->entries[i].key,
                           map->entries[i].as.string_value);
                } break;
                case MAP_TYPE_FUNCTION: {
                    printf("  '%s': [FunctionObject],\n", map->entries[i].key);
                } break;
                case MAP_TYPE_NULL:
                default: {
                    printf("  '%s': NULL,\n", map->entries[i].key);
                }
            }
        }
    }

    printf("}\n");
}