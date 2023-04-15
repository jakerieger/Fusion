// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef LUNA_INSTRUCTION_H
#define LUNA_INSTRUCTION_H

#include "ast.h"
#include "hash_map.h"

typedef enum {
    OP_PUSH,           // Push a value to the stack
    OP_LOAD,           // Load a reference value
    OP_POP,            // Pop a value from the stack
    OP_STORE,          // Store a reference value
    OP_ADD,            // Add two values and push the result to the stack
    OP_SUBTRACT,       // Subtract two values and push the result to the stack
    OP_MULTIPLY,       // Multiply two values and push the result to the stack
    OP_DIVIDE,         // Divide two values and push the result to the stack
    OP_MODULUS,        // Divide two values and push the remainder value to the stack
    OP_HEAP_ALLOC,     // Allocate memory on the heap
    OP_HEAP_FREE,      // Free memory on the heap
    OP_HALT,           // Halt and exit the program
    OP_NOOP,           // Do nothing
    OP_JUMP,           // Jump to a specified address
    OP_JUMP_IF_TRUE,   // Jump to a specified address if the top of the stack is true
    OP_JUMP_IF_FALSE,  // Jump to a specified address if the top of the stack is false
    OP_COMPARE_EQ,     // Compare the top two stack values for equality
    OP_COMPARE_NE,     // Compare the top two stack values for inequality
    OP_COMPARE_LT,     // Compare the top two stack values for less than
    OP_COMPARE_GT,     // Compare the top two stack values for greater than
    OP_COMPARE_LE,     // Compare the top two stack values for less than or equal to
    OP_COMPARE_GE,     // Compare the top two stack values for greater than or equal to
} OpCode;

typedef enum {
    OP_TYPE_STRING,
    OP_TYPE_NUMBER,
    OP_TYPE_BOOLEAN,
    OP_TYPE_SYMBOL,
    OP_TYPE_NULL,
} OperandType;

typedef struct {
    OpCode opcode;
    union {
        Number number_value;
        String string_value;
        Boolean boolean_value;
        char* symbol;
    } operand;
    OperandType operand_type;
} Instruction;

typedef struct InstructionStream {
    Instruction* instructions;
    int count;
} InstructionStream;

void generate_instructions(ExprNode* ast_root, InstructionStream* stream);
void emit_instruction(InstructionStream* stream, OpCode opcode, void* operand,
                      OperandType operand_type);

#endif
