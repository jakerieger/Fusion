// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#include "instruction.h"
#include <malloc.h>

void generate_instructions(ExprNode* ast_root, InstructionStream* stream) {
    switch (ast_root->type) {
        case EXPR_ADD:
            generate_instructions(ast_root->as.binary_op_node->left, stream);
            generate_instructions(ast_root->as.binary_op_node->right, stream);
            emit_instruction(stream, OP_ADD, NULL, OP_TYPE_NULL, ST_NULL);
            break;
        case EXPR_MULTIPLY:
            generate_instructions(ast_root->as.binary_op_node->left, stream);
            generate_instructions(ast_root->as.binary_op_node->right, stream);
            emit_instruction(stream, OP_MULTIPLY, NULL, OP_TYPE_NULL, ST_NULL);
            break;
        case EXPR_DIVIDE:
            generate_instructions(ast_root->as.binary_op_node->left, stream);
            generate_instructions(ast_root->as.binary_op_node->right, stream);
            emit_instruction(stream, OP_DIVIDE, NULL, OP_TYPE_NULL, ST_NULL);
            break;
        case EXPR_MODULUS:
            generate_instructions(ast_root->as.binary_op_node->left, stream);
            generate_instructions(ast_root->as.binary_op_node->right, stream);
            emit_instruction(stream, OP_MODULUS, NULL, OP_TYPE_NULL, ST_NULL);
            break;
        case EXPR_SUBTRACT:
            generate_instructions(ast_root->as.binary_op_node->left, stream);
            generate_instructions(ast_root->as.binary_op_node->right, stream);
            emit_instruction(stream, OP_SUBTRACT, NULL, OP_TYPE_NULL, ST_NULL);
            break;
        case EXPR_NUMBER:
            emit_instruction(stream, OP_PUSH, &(ast_root->as.number_node->value), OP_TYPE_NUMBER,
                             ST_NULL);
            break;
        case EXPR_STR:
            emit_instruction(stream, OP_PUSH, ast_root->as.string_node->value, OP_TYPE_STR,
                             ST_NULL);
            break;
        case EXPR_REFERENCE:
            emit_instruction(stream, OP_LOAD, ast_root->as.reference_node->name, OP_TYPE_SYMBOL,
                             ST_NULL);
            break;
        case EXPR_ASSIGN:
            // Generate instructions to evaluate the right-hand side expression
            generate_instructions(ast_root->as.assign_op_node->right, stream);

            // Generate instructions to load the value from the temporary location and
            // store it in the symbol table using the symbol name from the left-hand side
            char* symbol_name = ast_root->as.assign_op_node->left.reference_node->name;
            if (ast_root->as.assign_op_node->right->type != EXPR_STR) {
                emit_instruction(stream, OP_STORE, symbol_name, OP_TYPE_SYMBOL, ST_NUMBER);
            } else {
                emit_instruction(stream, OP_STORE, symbol_name, OP_TYPE_SYMBOL, ST_STR);
            }

            break;
    }
}
void emit_instruction(InstructionStream* stream, OpCode opcode, void* operand,
                      OperandType operand_type, StoreType store_type) {
    stream->count++;
    stream->instructions = realloc(stream->instructions, stream->count * sizeof(Instruction));
    stream->instructions[stream->count - 1].opcode = opcode;
    stream->instructions[stream->count - 1].operand_type = operand_type;

    if (operand_type == OP_TYPE_NUMBER) {
        stream->instructions[stream->count - 1].operand.number = *((double*) operand);
    } else if (operand_type == OP_TYPE_SYMBOL || operand_type == OP_TYPE_STR) {
        stream->instructions[stream->count - 1].operand.symbol = (char*) operand;
    }

    if (opcode == OP_STORE) { stream->instructions[stream->count - 1].store_type = store_type; }
}
