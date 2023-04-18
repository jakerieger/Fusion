// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#include "instruction.h"
#include "repl.h"
#include <malloc.h>
#include <stdlib.h>

void generate_instructions(ExprNode* ast_root, InstructionStream* stream) {
    switch (ast_root->type) {
        case EXPR_ADD: {
            generate_instructions(ast_root->as.binary_op_node->left, stream);
            generate_instructions(ast_root->as.binary_op_node->right, stream);
            OperandContext context = {.type = OP_TYPE_NULL, .scope = SCOPE_GLOBAL};
            emit_instruction(stream, OP_ADD, NULL, context);
        } break;
        case EXPR_MULTIPLY: {
            generate_instructions(ast_root->as.binary_op_node->left, stream);
            generate_instructions(ast_root->as.binary_op_node->right, stream);
            OperandContext context = {.type = OP_TYPE_NULL, .scope = SCOPE_GLOBAL};
            emit_instruction(stream, OP_MULTIPLY, NULL, context);
        } break;
        case EXPR_DIVIDE: {
            generate_instructions(ast_root->as.binary_op_node->left, stream);
            generate_instructions(ast_root->as.binary_op_node->right, stream);
            OperandContext context = {.type = OP_TYPE_NULL, .scope = SCOPE_GLOBAL};
            emit_instruction(stream, OP_DIVIDE, NULL, context);
        } break;
        case EXPR_MODULUS: {
            generate_instructions(ast_root->as.binary_op_node->left, stream);
            generate_instructions(ast_root->as.binary_op_node->right, stream);
            OperandContext context = {.type = OP_TYPE_NULL, .scope = SCOPE_GLOBAL};
            emit_instruction(stream, OP_MODULUS, NULL, context);
        } break;
        case EXPR_SUBTRACT: {
            generate_instructions(ast_root->as.binary_op_node->left, stream);
            generate_instructions(ast_root->as.binary_op_node->right, stream);
            OperandContext context = {.type = OP_TYPE_NULL, .scope = SCOPE_GLOBAL};
            emit_instruction(stream, OP_SUBTRACT, NULL, context);
        } break;
        case EXPR_NUMBER: {
            OperandContext context = {.type = OP_TYPE_NUMBER, .scope = SCOPE_GLOBAL};
            emit_instruction(stream, OP_PUSH, &(ast_root->as.number_node->value), context);
        } break;
        case EXPR_STRING: {
            OperandContext context = {.type = OP_TYPE_STRING, .scope = SCOPE_GLOBAL};
            emit_instruction(stream, OP_PUSH, ast_root->as.string_node->value, context);
        } break;
        case EXPR_BOOLEAN: {
            OperandContext context = {.type = OP_TYPE_BOOLEAN, .scope = SCOPE_GLOBAL};
            emit_instruction(stream, OP_PUSH, &ast_root->as.boolean_node->value, context);
        } break;
        case EXPR_REFERENCE: {
            OperandContext context = {.type = OP_TYPE_SYMBOL, .scope = SCOPE_GLOBAL};
            emit_instruction(stream, OP_LOAD, ast_root->as.reference_node->name, context);
        } break;
        case EXPR_ASSIGN: {
            OperandContext context = {.type = OP_TYPE_SYMBOL, .scope = SCOPE_GLOBAL};
            // Generate instructions to evaluate the right-hand side expression
            generate_instructions(ast_root->as.assign_op_node->right, stream);

            // Generate instructions to load the value from the temporary location and
            // store it in the symbol table using the symbol name from the left-hand side
            char* symbol_name = ast_root->as.assign_op_node->left.reference_node->name;
            emit_instruction(stream, OP_STORE, symbol_name, context);
        } break;
        case EXPR_FUNC_CALL: {
            OperandContext context = {.type = OP_TYPE_SYMBOL, .scope = SCOPE_GLOBAL};
            for (int i = 0; i < ast_root->as.function_call_node->argc; i++) {
                generate_instructions(&ast_root->as.function_call_node->args[i], stream);
            }

            emit_instruction(stream, OP_CALL, ast_root->as.function_call_node->name, context);

            context.type = OP_TYPE_NULL;
            emit_instruction(stream, OP_POP, NULL, context);
            for (int i = 0; i < ast_root->as.function_call_node->argc; i++) {
                emit_instruction(stream, OP_POP, NULL, context);
            }
        } break;
        case EXPR_FUNC_DEF: {
            OperandContext context = {.type = OP_TYPE_NULL, .scope = SCOPE_GLOBAL};
            FunctionObject* func = malloc(sizeof(FunctionObject));
            func->name = ast_root->as.function_def_node->name;
            func->args_names = ast_root->as.function_def_node->parameters->symbols;
            func->return_type = ast_root->as.function_def_node->return_type;
            func->argc = ast_root->as.function_def_node->argc;

            // Emit instructions to store params
            for (int i = 0; i < func->argc; i++) {
                emit_instruction(stream, OP_STORE_PARAM,
                                 ast_root->as.function_def_node->parameters->symbols[i], context);
            }
            context.type = OP_TYPE_FUNCTION;
            emit_instruction(stream, OP_NEW_FUNC, func, context);
            func->entry_point = stream->count + 1;
            generate_instructions(ast_root->as.function_def_node->body, stream);
            context.type = OP_TYPE_NULL;
            emit_instruction(stream, OP_RETURN, NULL, context);
        } break;
    }
}

void emit_instruction(InstructionStream* stream, OpCode opcode, void* operand,
                      OperandContext operand_context) {
    stream->count++;
    stream->instructions = realloc(stream->instructions, stream->count * sizeof(Instruction));
    stream->instructions[stream->count - 1].opcode = opcode;
    stream->instructions[stream->count - 1].operand_context = operand_context;

    switch (operand_context.type) {
        case OP_TYPE_NUMBER:
            stream->instructions[stream->count - 1].operand.number_value =
                    *((FusionNumber*) operand);
            break;
        case OP_TYPE_SYMBOL:
            stream->instructions[stream->count - 1].operand.symbol = (char*) operand;
            break;
        case OP_TYPE_BOOLEAN:
            stream->instructions[stream->count - 1].operand.boolean_value =
                    *((FusionBoolean*) operand);
            break;
        case OP_TYPE_FUNCTION:
            stream->instructions[stream->count - 1].operand.function_object =
                    *((FunctionObject*) operand);
            break;
        case OP_TYPE_NULL:
            break;
        case OP_TYPE_STRING:
            stream->instructions[stream->count - 1].operand.string_value = (FusionString) operand;
            break;
    }
}
