// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef LUNA_VM_H
#define LUNA_VM_H

#include "call_stack.h"
#include "hash_map.h"
#include "heap.h"
#include "instruction.h"
#include "lexer.h"
#include "perf_hash_map.h"
#include "register.h"
#include "stack.h"
#include "vmconfig.h"
#include <stdlib.h>

typedef struct VM {
    VMConfig* config;
    Heap* heap;
    Stack* stack;
    CallStack* call_stack;
    HashMap* symbol_table;
    PHashMap* function_table;
    Int64Register register_i64;
    Int32Register register_i32;
    Int16Register register_i16;
    Int8Register register_i8;
    StrRegister register_str;
    int stack_ptr;
    int call_stack_ptr;
    int heap_ptr;
    int instr_ptr;
} VM;

VM* initialize_vm();
int configure_vm(VM* vm);
int reset_vm(VM* vm);
int shutdown_vm(VM* vm);
int run_program(VM* vm, InstructionStream* stream);
char** generate_vm_assembly(VM* vm, InstructionStream* stream);

#endif