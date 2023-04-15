// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#include "stack.h"
#include "repl.h"
#include <memory.h>

Stack* create_stack(int size) {
    Stack* stack = malloc(sizeof(Stack));
    if (stack == NULL) { return NULL; }

    stack->entries = malloc(size * sizeof(StackEntry));
    if (stack->entries == NULL) {
        free(stack);
        return NULL;
    }

    stack->max_size = size;
    stack->count = 0;

    return stack;
}

void push(Stack* stack, void* value, char type) {
    if (stack->count < stack->max_size) {
        if (type == 0) {
            stack->entries[stack->count].type = type;
            stack->entries[stack->count].value.d_val = *((double*) value);
            stack->count++;
        } else if (type == 1) {
            stack->entries[stack->count].type = type;
            stack->entries[stack->count].value.s_val = (char*) value;
            stack->count++;
        } else {
            // Error: Unsupported value type (double or char* only)
            print_error("Tried pushing value with unsupported size to stack\n");
            return;
        }
    } else {
        // Error: Stack Overflow
        print_error("Stack overflow\n");
        exit(2);
    }
}

void* pop(Stack* stack, char type) {
    if (stack->count > 0) {
        //        if (stack->count == 0) {
        //            // Reset the stack to avoid memory leaks
        //            memset(stack->entries, 0, sizeof(StackEntry) * stack->max_size);
        //        }
        if (type == 0) {
            return &stack->entries[--stack->count].value.d_val;
        } else if (type == 1) {
            return stack->entries[--stack->count].value.s_val;
        } else {
            return NULL;
        }
    } else {
        // Error: Stack underflow
        return NULL;
    }
}
