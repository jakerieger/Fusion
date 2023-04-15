// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef LUNA_STACK_H
#define LUNA_STACK_H

#include <stdlib.h>

typedef struct {
    union {
        double d_val;
        char* s_val;
    } value;
    char type;  // 0 for double, 1 for char*
} StackEntry;

typedef struct {
    int count;
    int max_size;
    StackEntry* entries;
} Stack;

Stack* create_stack(int size);

void push(Stack* stack, void* value, char type);

void* pop(Stack* stack, char type);

#endif