// Copyright © 2023 Jake Rieger.
//
// Licensed under the GNU General Public License v3.0

/**
 * @file vm.h
 *
 * @brief This file defines the structure and functions for a virtual machine for the Fusion programming language.
 */


#ifndef FUSION_VM_H
#define FUSION_VM_H

#include "call_stack.h"
#include "hashmap.h"
#include "heap.h"
#include "instruction.h"
#include "lexer.h"
#include "register.h"
#include "stack.h"
#include "symbol_table.h"
#include "vmconfig.h"
#include <stdlib.h>


/**
 * @brief A virtual machine for the Fusion programming language.
 */
typedef struct VM {
    VMConfig* config; /**< The configuration for the VM. */
    Heap* heap; /**< The heap used for memory allocation. */
    Stack* stack; /**< The stack used for storing values during execution. */
    CallStack* call_stack; /**< The call stack used for function calls. */
    SymbolTableStack* symbol_table_stack; /**< The symbol table stack used for variable lookup. */
    HashMap* function_table; /**< The hash map used for storing function definitions. */
    Int64Register register_i64; /**< The 64-bit integer register. */
    Int32Register register_i32; /**< The 32-bit integer register. */
    Int16Register register_i16; /**< The 16-bit integer register. */
    Int8Register register_i8; /**< The 8-bit integer register. */
    StrRegister register_str; /**< The string register. */
    int call_stack_ptr; /**< The call stack pointer. */
    int heap_ptr; /**< The heap pointer. */
    int instr_ptr; /**< The instruction pointer. */
} VM;

/**
 * @brief Initializes a new VM instance.
 *
 * @return A pointer to the new VM instance.
 */
VM* initialize_vm();

/**
 * @brief Configures the specified VM instance.
 *
 * @param vm The VM instance to configure.
 *
 * @return 0 if successful, otherwise an error code.
 */
int configure_vm(VM* vm);

/**
 * @brief Resets the specified VM instance to its initial state.
 *
 * @param vm The VM instance to reset.
 *
 * @return 0 if successful, otherwise an error code.
 */
int reset_vm(VM* vm);

/**
 * @brief Shuts down the specified VM instance.
 *
 * @param vm The VM instance to shut down.
 *
 * @return 0 if successful, otherwise an error code.
 */
int shutdown_vm(VM* vm);

/**
 * @brief Runs the specified program on the specified VM instance.
 *
 * @param vm The VM instance to use for execution.
 * @param stream The instruction stream containing the program to execute.
 * @param proc_name The name of the process being executed.
 *
 * @return 0 if successful, otherwise an error code.
 */
int run_program(VM* vm, InstructionStream* stream, const char* proc_name);

/**
 * @brief Generates assembly code for the specified program and VM instance.
 *
 * @param vm The VM instance to use for assembly code generation.
 * @param stream The instruction stream containing the program to generate assembly code for.
 * @param proc_name The name of the process to generate assembly code for.
 *
 * @return An array of strings containing the assembly code.
 */
char** generate_vm_assembly(VM* vm, InstructionStream* stream, const char* proc_name);
#endif