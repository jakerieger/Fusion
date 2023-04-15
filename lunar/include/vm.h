// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef LUNA_VM_H
#define LUNA_VM_H

#include "hash_map.h"
#include "heap.h"
#include "instruction.h"
#include "lexer.h"
#include "register.h"
#include "stack.h"
#include "vmconfig.h"
#include <stdlib.h>

typedef struct VM {
    VMConfig* config;
    Heap* heap;
    Stack* stack;
    HashMap* symbol_table;
    Int64Register register_u64;
    Int32Register register_u32;
    StrRegister register_str;
    int stack_ptr;
    int heap_ptr;
    int instr_ptr;
} VM;

VM* initialize_vm();
int configure_vm(VM* vm);
int reset_vm(VM* vm);
int shutdown_vm(VM* vm);
int run_program(VM* vm, InstructionStream* stream);
int* generate_byte_code(VM* vm, InstructionStream* stream);

#endif