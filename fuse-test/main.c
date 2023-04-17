#include "data_structs/hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    HashMap* map = hashmap_create();

    FusionNumber val_1 = 102;
    FusionNumber val_2 = 3.14159;
    FusionString val_3 = "hello";
    FusionString val_4 = "world";
    FusionBoolean val_5 = {.value = true};
    FusionBoolean val_6 = {.value = false};
    FunctionObject val_7 = {};
    FunctionObject val_8 = {};
    FusionNumber val_9 = 42;
    FusionNumber val_10 = -10;
    FusionString val_11 = "foo";
    FusionString val_12 = "bar";
    FusionBoolean val_13 = {.value = true};
    FusionBoolean val_14 = {.value = false};
    FunctionObject val_15 = {};
    FunctionObject val_16 = {};
    FusionNumber val_17 = 3.5;
    FusionNumber val_18 = -5.5;
    FusionString val_19 = "baz";
    FusionString val_20 = "qux";
    FusionBoolean val_21 = {.value = true};
    FusionBoolean val_22 = {.value = false};
    FunctionObject val_23 = {};
    FunctionObject val_24 = {};
    FusionNumber val_25 = 0;
    FusionNumber val_26 = 1.23;
    FusionString val_27 = "hello2";
    FusionString val_28 = "world2";
    FusionBoolean val_29 = {.value = true};
    FusionBoolean val_30 = {.value = false};
    FunctionObject val_31 = {};
    FunctionObject val_32 = {};
    FusionNumber val_33 = 999;
    FusionNumber val_34 = -999;
    FusionString val_35 = "cat";
    FusionString val_36 = "dog";
    FusionBoolean val_37 = {.value = true};
    FusionBoolean val_38 = {.value = false};
    FunctionObject val_39 = {};

    hashmap_put(map, "val_1", &val_1, MAP_TYPE_NUMBER);
    hashmap_put(map, "val_2", &val_2, MAP_TYPE_NUMBER);
    hashmap_put(map, "val_3", val_3, MAP_TYPE_STRING);
    hashmap_put(map, "val_4", val_4, MAP_TYPE_STRING);
    hashmap_put(map, "val_5", &val_5, MAP_TYPE_BOOLEAN);
    hashmap_put(map, "val_6", &val_6, MAP_TYPE_BOOLEAN);
    hashmap_put(map, "val_7", &val_7, MAP_TYPE_FUNCTION);
    hashmap_put(map, "val_8", &val_8, MAP_TYPE_FUNCTION);
    hashmap_put(map, "val_9", &val_9, MAP_TYPE_NUMBER);
    hashmap_put(map, "val_10", &val_10, MAP_TYPE_NUMBER);
    hashmap_put(map, "val_11", val_11, MAP_TYPE_STRING);
    hashmap_put(map, "val_12", val_12, MAP_TYPE_STRING);
    hashmap_put(map, "val_13", &val_13, MAP_TYPE_BOOLEAN);
    hashmap_put(map, "val_14", &val_14, MAP_TYPE_BOOLEAN);
    hashmap_put(map, "val_15", &val_15, MAP_TYPE_FUNCTION);
    hashmap_put(map, "val_16", &val_16, MAP_TYPE_FUNCTION);
    hashmap_put(map, "val_17", &val_17, MAP_TYPE_NUMBER);
    hashmap_put(map, "val_18", &val_18, MAP_TYPE_NUMBER);
    hashmap_put(map, "val_19", val_19, MAP_TYPE_STRING);
    hashmap_put(map, "val_20", val_20, MAP_TYPE_STRING);
    hashmap_put(map, "val_21", &val_21, MAP_TYPE_BOOLEAN);
    hashmap_put(map, "val_22", &val_22, MAP_TYPE_BOOLEAN);
    hashmap_put(map, "val_23", &val_23, MAP_TYPE_FUNCTION);
    hashmap_put(map, "val_24", &val_24, MAP_TYPE_FUNCTION);
    hashmap_put(map, "val_25", &val_25, MAP_TYPE_NUMBER);
    hashmap_put(map, "val_26", &val_26, MAP_TYPE_NUMBER);
    hashmap_put(map, "val_27", val_27, MAP_TYPE_STRING);
    hashmap_put(map, "val_28", val_28, MAP_TYPE_STRING);
    hashmap_put(map, "val_29", &val_29, MAP_TYPE_BOOLEAN);
    hashmap_put(map, "val_30", &val_30, MAP_TYPE_BOOLEAN);
    hashmap_put(map, "val_31", &val_31, MAP_TYPE_FUNCTION);
    hashmap_put(map, "val_32", &val_32, MAP_TYPE_FUNCTION);
    hashmap_put(map, "val_33", &val_33, MAP_TYPE_NUMBER);
    hashmap_put(map, "val_34", &val_34, MAP_TYPE_NUMBER);
    hashmap_put(map, "val_35", val_35, MAP_TYPE_STRING);
    hashmap_put(map, "val_36", val_36, MAP_TYPE_STRING);
    hashmap_put(map, "val_37", &val_37, MAP_TYPE_BOOLEAN);
    hashmap_put(map, "val_38", &val_38, MAP_TYPE_BOOLEAN);
    hashmap_put(map, "val_39", &val_39, MAP_TYPE_FUNCTION);

    // print_hashmap(map);

    clock_t start = clock();
    for (int i = 0; i < 39; i++) {
        int random_num = rand() % 39 + 1;
        char key[10];
        sprintf(key, "val_%d", random_num);

        Entry* value = hashmap_get(map, key);

        printf("%s = %f\n", key, value->as.number_value);
    }
    clock_t end = clock();

    // Calculate and print the reads per second
    double elapsed_time = ((double) (end - start)) / CLOCKS_PER_SEC;
    double reads_per_second = 39 / elapsed_time;
    printf("Reads per second: %f\n", reads_per_second);

    // Cleanup
    hashmap_destroy(map);

    return 0;
}