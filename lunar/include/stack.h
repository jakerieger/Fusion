// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef LUNA_STACK_H
#define LUNA_STACK_H

#include "types.h"
#include <stdlib.h>

typedef enum FrameType { FT_BOOLEAN, FT_NUMBER, FT_STRING, FT_NULL } FrameType;

typedef struct {
    union {
        Boolean boolean_value;
        Number number_value;
        String string_value;
    } value;
    FrameType type;
} StackEntry;

typedef struct {
    int count;
    int max_size;
    StackEntry* entries;
} Stack;

Stack* create_stack(int size);

void push(Stack* stack, void* value, FrameType type);

void* pop(Stack* stack, FrameType* type);

#endif