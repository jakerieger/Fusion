// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef LUNA_STACK_H
#define LUNA_STACK_H

#include "types.h"
#include <stdlib.h>

typedef struct {
    union {
        LunaBoolean boolean_value;
        LunaNumber number_value;
        LunaString string_value;
    } value;
    LunaType type;
} StackEntry;

typedef struct {
    int count;
    int max_size;
    StackEntry* entries;
} Stack;

Stack* create_stack(int size);

void push(Stack* stack, void* value, LunaType type);

void* pop(Stack* stack, LunaType* type);

#endif