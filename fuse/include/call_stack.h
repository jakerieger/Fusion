// Copyright © 2023 Jake Rieger.
//
// Licensed under the GNU General Public License v3.0

#ifndef FUSION_CALL_STACK_H
#define FUSION_CALL_STACK_H

#include "types.h"
#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

typedef union ArgValue {
    FusionNumber num_args_value;    // Pointer to array of numeric arguments
    FusionBoolean bool_args_value;  // Pointer to array of boolean arguments
    FusionString str_args_value;    // Pointer to array of string arguments
} ArgValue;

typedef struct CallFrame {
    int return_address;
    int argc;
    ArgValue* args_values;  // Pointer to the union, which can hold any of the above types
} CallFrame;

typedef struct CallStack {
    CallFrame* frames;
    uint32_t frame_count;
    uint32_t frame_capacity;
} CallStack;

CallFrame* create_call_frame(int return_address, int argc);

void destroy_call_frame(CallFrame* frame);

CallStack* create_call_stack(uint32_t capacity);

void destroy_call_stack(CallStack* stack);

void push_call_frame(CallStack* stack, int return_address, int argc);

CallFrame pop_call_frame(CallStack* stack);

CallFrame* peek_call_frame(CallStack* stack);

void set_call_frame_args(CallStack* stack, ArgValue* args_values, int argc);

#endif  //FUSION_CALL_STACK_H
