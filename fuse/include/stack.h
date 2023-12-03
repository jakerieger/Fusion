// Copyright © 2023 Jake Rieger.
//
// Licensed under the GNU General Public License v3.0

#ifndef FUSION_STACK_H
#define FUSION_STACK_H

#include "types.h"
#include <stdlib.h>

typedef struct {
    union {
        FusionBoolean boolean_value;
        FusionNumber number_value;
        FusionString string_value;
    } value;
    FusionType type;
} StackEntry;

typedef struct {
    int count;
    int max_size;
    StackEntry* entries;
} Stack;

Stack* create_stack(int size);

void push(Stack* stack, void* value, FusionType type);

void* pop(Stack* stack, FusionType* type);

#endif