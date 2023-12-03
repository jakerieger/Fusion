// Copyright © 2023 Jake Rieger.
//
// Licensed under the GNU General Public License v3.0

//
// Created by jr on 4/12/23.
//

#include "call_stack.h"

CallFrame* create_call_frame(int return_address, int argc) {
    CallFrame* frame = (CallFrame*) malloc(sizeof(CallFrame));
    frame->return_address = return_address;
    frame->argc = argc;
    frame->args_values = NULL;
    return frame;
}

void destroy_call_frame(CallFrame* frame) {
    free(frame->args_values);
    free(frame);
}

CallStack* create_call_stack(uint32_t capacity) {
    CallStack* stack = (CallStack*) malloc(sizeof(CallStack));
    stack->frames = (CallFrame*) malloc(sizeof(CallFrame) * capacity);
    stack->frame_count = 0;
    stack->frame_capacity = capacity;
    return stack;
}

void destroy_call_stack(CallStack* stack) {
    for (uint32_t i = 0; i < stack->frame_count; i++) { destroy_call_frame(&stack->frames[i]); }
    free(stack->frames);
    free(stack);
}

void push_call_frame(CallStack* stack, int return_address, int argc) {
    if (stack->frame_count == stack->frame_capacity) {
        // The stack is full, so we need to resize it
        stack->frame_capacity *= 2;
        stack->frames =
                (CallFrame*) realloc(stack->frames, sizeof(CallFrame) * stack->frame_capacity);
    }
    stack->frames[stack->frame_count].return_address = return_address;
    stack->frames[stack->frame_count].argc = argc;
    stack->frames[stack->frame_count].args_values = NULL;
    stack->frame_count++;
}

CallFrame pop_call_frame(CallStack* stack) {
    CallFrame frame = stack->frames[--stack->frame_count];
    stack->frames[stack->frame_count].args_values = NULL;
    return frame;
}

CallFrame* peek_call_frame(CallStack* stack) {
    if (stack->frame_count == 0) { return NULL; }

    return &stack->frames[stack->frame_count - 1];
}

void set_call_frame_args(CallStack* stack, ArgValue* args_values, int argc) {
    CallFrame* frame = peek_call_frame(stack);
    if (frame == NULL) { return; }

    frame->args_values = (ArgValue*) malloc(sizeof(ArgValue));
    memmove(frame->args_values, args_values, sizeof(ArgValue) * argc);
}