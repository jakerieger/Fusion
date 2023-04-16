// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#include "instruction.h"
#include "repl.h"
#include <malloc.h>
#include <stdlib.h>

void generate_instructions(ExprNode* ast_root, InstructionStream* stream) {
    switch (ast_root->type) {
        case EXPR_ADD:
            generate_instructions(ast_root->as.binary_op_node->left, stream);
            generate_instructions(ast_root->as.binary_op_node->right, stream);
            emit_instruction(stream, OP_ADD, NULL, OP_TYPE_NULL);
            break;
        case EXPR_MULTIPLY:
            generate_instructions(ast_root->as.binary_op_node->left, stream);
            generate_instructions(ast_root->as.binary_op_node->right, stream);
            emit_instruction(stream, OP_MULTIPLY, NULL, OP_TYPE_NULL);
            break;
        case EXPR_DIVIDE:
            generate_instructions(ast_root->as.binary_op_node->left, stream);
            generate_instructions(ast_root->as.binary_op_node->right, stream);
            emit_instruction(stream, OP_DIVIDE, NULL, OP_TYPE_NULL);
            break;
        case EXPR_MODULUS:
            generate_instructions(ast_root->as.binary_op_node->left, stream);
            generate_instructions(ast_root->as.binary_op_node->right, stream);
            emit_instruction(stream, OP_MODULUS, NULL, OP_TYPE_NULL);
            break;
        case EXPR_SUBTRACT:
            generate_instructions(ast_root->as.binary_op_node->left, stream);
            generate_instructions(ast_root->as.binary_op_node->right, stream);
            emit_instruction(stream, OP_SUBTRACT, NULL, OP_TYPE_NULL);
            break;
        case EXPR_NUMBER:
            emit_instruction(stream, OP_PUSH, &(ast_root->as.number_node->value), OP_TYPE_NUMBER);
            break;
        case EXPR_STRING:
            emit_instruction(stream, OP_PUSH, ast_root->as.string_node->value, OP_TYPE_STRING);
            break;
        case EXPR_BOOLEAN:
            emit_instruction(stream, OP_PUSH, &ast_root->as.boolean_node->value, OP_TYPE_BOOLEAN);
            break;
        case EXPR_REFERENCE:
            emit_instruction(stream, OP_LOAD, ast_root->as.reference_node->name, OP_TYPE_SYMBOL);
            break;
        case EXPR_ASSIGN:
            // Generate instructions to evaluate the right-hand side expression
            generate_instructions(ast_root->as.assign_op_node->right, stream);

            // Generate instructions to load the value from the temporary location and
            // store it in the symbol table using the symbol name from the left-hand side
            // TODO: Figure out a way to handle ExprNodes that aren't a direct value reference
            char* symbol_name = ast_root->as.assign_op_node->left.reference_node->name;
            emit_instruction(stream, OP_STORE, symbol_name, OP_TYPE_SYMBOL);
            break;
        case EXPR_FUNC_CALL:
            for (int i = 0; i < ast_root->as.function_call_node->argc; i++) {
                generate_instructions(&ast_root->as.function_call_node->args[i], stream);
            }

            emit_instruction(stream, OP_CALL, ast_root->as.function_call_node->name,
                             OP_TYPE_SYMBOL);

            emit_instruction(stream, OP_POP, NULL, OP_TYPE_NULL);
            for (int i = 0; i < ast_root->as.function_call_node->argc; i++) {
                emit_instruction(stream, OP_POP, NULL, OP_TYPE_NULL);
            }

            break;
        case EXPR_FUNC_DEF: {
            FunctionObject* func = malloc(sizeof(FunctionObject));
            func->name = ast_root->as.function_def_node->name;
            func->args_names = ast_root->as.function_def_node->parameters->symbols;
            func->return_type = ast_root->as.function_def_node->return_type;
            func->argc = ast_root->as.function_def_node->argc;

            emit_instruction(stream, OP_NEW_FUNC, func, OP_TYPE_FUNCTION);
            emit_instruction(stream, OP_STORE, ast_root->as.function_def_node->name,
                             OP_TYPE_SYMBOL);

            generate_instructions(ast_root->as.function_def_node->body, stream);
            emit_instruction(stream, OP_RETURN, NULL, OP_TYPE_NULL);
        } break;
    }
}

void emit_instruction(InstructionStream* stream, OpCode opcode, void* operand,
                      OperandType operand_type) {
    stream->count++;
    stream->instructions = realloc(stream->instructions, stream->count * sizeof(Instruction));
    stream->instructions[stream->count - 1].opcode = opcode;
    stream->instructions[stream->count - 1].operand_type = operand_type;

    switch (operand_type) {
        case OP_TYPE_NUMBER:
            stream->instructions[stream->count - 1].operand.number_value = *((LunaNumber*) operand);
            break;
        case OP_TYPE_SYMBOL:
            stream->instructions[stream->count - 1].operand.symbol = (char*) operand;
            break;
        case OP_TYPE_BOOLEAN:
            stream->instructions[stream->count - 1].operand.boolean_value =
                    *((LunaBoolean*) operand);
            break;
        case OP_TYPE_FUNCTION:
            stream->instructions[stream->count - 1].operand.function_object =
                    *((FunctionObject*) operand);
            break;
        case OP_TYPE_NULL:
            break;
        case OP_TYPE_STRING:
            stream->instructions[stream->count - 1].operand.string_value = (LunaString) operand;
            break;
    }
}
