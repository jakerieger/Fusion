// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#include "ast.h"
#include "error.h"
#include "lexer.h"
#include "repl.h"
#include "token_stream.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_IS(__token, __type) __token == __type

ExprNode* parse_expr(TokenStream* tokens) { return parse_add_expr(tokens); }

ExprNode* parse_add_expr(TokenStream* tokens) {
    ExprNode* left = parse_mul_expr(tokens);

    while (TOKEN_IS(peek_token(tokens)->type, TOKEN_PLUS) ||
           TOKEN_IS(peek_token(tokens)->type, TOKEN_MINUS)) {
        Token* op = peek_token(tokens);
        advance_token(tokens);
        ExprNode* right = parse_mul_expr(tokens);

        BinaryOpNode* binary_op_node = malloc(sizeof(BinaryOpNode));
        binary_op_node->left = left;
        binary_op_node->right = right;

        ExprNode* add_expr_node = malloc(sizeof(ExprNode));
        add_expr_node->type = op->type == TOKEN_PLUS ? EXPR_ADD : EXPR_SUBTRACT;
        add_expr_node->as.binary_op_node = binary_op_node;

        left = add_expr_node;
    }

    return left;
}

ExprNode* parse_mul_expr(TokenStream* tokens) {
    ExprNode* left = parse_primary_expr(tokens);

    while (TOKEN_IS(peek_token(tokens)->type, TOKEN_MULTIPLY) ||
           TOKEN_IS(peek_token(tokens)->type, TOKEN_DIVIDE) ||
           TOKEN_IS(peek_token(tokens)->type, TOKEN_MODULO)) {
        Token* op = peek_token(tokens);
        advance_token(tokens);
        ExprNode* right = parse_primary_expr(tokens);

        BinaryOpNode* binary_op_node = malloc(sizeof(BinaryOpNode));
        binary_op_node->left = left;
        binary_op_node->right = right;

        ExprNode* mul_expr_node = malloc(sizeof(ExprNode));
        mul_expr_node->type = op->type == TOKEN_MULTIPLY ? EXPR_MULTIPLY
                              : op->type == TOKEN_DIVIDE ? EXPR_DIVIDE
                                                         : EXPR_MODULUS;

        mul_expr_node->as.binary_op_node = binary_op_node;
        left = mul_expr_node;
    }

    return left;
}

ExprNode* parse_identifier_expr(TokenStream* tokens) {
    Token* token = peek_token(tokens);
    char* name = (char*) token->val;
    advance_token(tokens);

    if (peek_token(tokens)->type == TOKEN_EQUALS) {
        advance_token(tokens);  // consume the '=' token
        ExprNode* right = parse_expr(tokens);

        AssignmentOpNode* assign_op_node = malloc(sizeof(AssignmentOpNode));
        assign_op_node->is_reference = false;
        assign_op_node->left.reference_node = malloc(sizeof(ReferenceNode));
        assign_op_node->left.reference_node->name = name;
        assign_op_node->right = right;

        ExprNode* assign_expr_node = malloc(sizeof(ExprNode));
        assign_expr_node->type = EXPR_ASSIGN;
        assign_expr_node->as.assign_op_node = assign_op_node;

        return assign_expr_node;
    } else if (peek_token(tokens)->type == TOKEN_LPAREN) {
        // This is a function call
        advance_token(tokens);

        // TODO: Parse provided arguments
        int argc = 0;
        ExprNode** args = malloc(sizeof(ExprNode*) + 1);
        CHECK_MEM(args)
        while (peek_token(tokens)->type != TOKEN_RPAREN) {
            if (peek_token(tokens)->type == TOKEN_COMMA &&
                peek_next_token(tokens)->type == TOKEN_RPAREN) {
                print_error("Unterminated argument list in function call (trailing ',').");
                exit(1);
            }

            if (peek_token(tokens)->type == TOKEN_COMMA) {
                advance_token(tokens);
                continue;
            }

            args = realloc(args, sizeof(ExprNode*) * (argc + 1));
            args[argc] = parse_expr(tokens);
            argc++;
        }

        advance_token(tokens);

        FunctionCallNode* func_call_node = malloc(sizeof(FunctionCallNode));
        func_call_node->argc = argc;
        func_call_node->name = name;
        func_call_node->args = args;

        ExprNode* func_call_expr_node = malloc(sizeof(ExprNode));
        func_call_expr_node->type = EXPR_FUNC_CALL;
        func_call_expr_node->as.function_call_node = func_call_node;

        return func_call_expr_node;
    } else {
        // This is a reference
        ReferenceNode* ref_node = malloc(sizeof(ReferenceNode));
        ref_node->name = (char*) name;

        ExprNode* ref_expr_node = malloc(sizeof(ExprNode));
        ref_expr_node->type = EXPR_REFERENCE;
        ref_expr_node->as.reference_node = ref_node;

        return ref_expr_node;
    }
}

ExprNode* parse_primary_expr(TokenStream* tokens) {
    Token* token = peek_token(tokens);

    if (token->type == TOKEN_NUMBER) {
        return parse_number_expr(tokens);
    } else if (token->type == TOKEN_STRING) {
        return parse_str_expr(tokens);
    } else if (token->type == TOKEN_BOOLEAN) {
        return parse_bool_expr(tokens);
    } else if (token->type == TOKEN_IDENTIFIER) {
        return parse_identifier_expr(tokens);
    } else if (token->type == TOKEN_KEYWORD) {
        return parse_keyword_expr(tokens);
    }
}

ExprNode* parse_number_expr(TokenStream* tokens) {
    Token* token = peek_token(tokens);
    advance_token(tokens);

    NumberNode* number_node = malloc(sizeof(NumberNode));
    number_node->value = *(double*) token->val;

    ExprNode* number_expr_node = malloc(sizeof(ExprNode));
    number_expr_node->type = EXPR_NUMBER;
    number_expr_node->as.number_node = number_node;

    return number_expr_node;
}

ExprNode* parse_str_expr(TokenStream* tokens) {
    Token* token = peek_token(tokens);
    advance_token(tokens);

    StringNode* str_node = malloc(sizeof(StringNode));
    str_node->value = (char*) token->val;

    ExprNode* str_expr_node = malloc(sizeof(ExprNode));
    str_expr_node->type = EXPR_STRING;
    str_expr_node->as.string_node = str_node;

    return str_expr_node;
}

ExprNode* parse_bool_expr(TokenStream* tokens) {
    Token* token = peek_token(tokens);
    advance_token(tokens);

    BooleanNode* bool_node = malloc(sizeof(BooleanNode));
    bool_node->value = *(FusionBoolean*) token->val;

    ExprNode* bool_expr_node = malloc(sizeof(ExprNode));
    bool_expr_node->type = EXPR_BOOLEAN;
    bool_expr_node->as.boolean_node = bool_node;

    return bool_expr_node;
}

ExprNode* parse_keyword_expr(TokenStream* tokens) {
    Token* token = peek_token(tokens);
    advance_token(tokens);

    if (strcmp((char*) token->val, "impl") == 0) { return parse_func_def_expr(tokens); }

    return NULL;
}

ExprNode* parse_func_def_expr(TokenStream* tokens) {
    if (peek_token(tokens)->type != TOKEN_IDENTIFIER) {
        print_error("Function definition missing name.\n");
        exit(1);
    }

    char* func_name = (char*) peek_token(tokens)->val;
    advance_token(tokens);

    if (peek_token(tokens)->type != TOKEN_LPAREN) {
        print_error("Function definition missing argument list opening paren '('\n");
        exit(1);
    }

    ArgumentListNode* args_list_node = malloc(sizeof(ArgumentListNode));
    if (args_list_node == NULL) {
        print_error("Failed to allocate memory for argument list node\n");
        exit(1);
    }

    args_list_node->symbols = malloc(sizeof(char*));
    if (args_list_node->symbols == NULL) {
        print_error("Failed to allocate memory for argument list array\n");
        exit(1);
    }
    int argc = 0;

    advance_token(tokens);
    while (peek_token(tokens)->type != TOKEN_RPAREN) {
        if (peek_token(tokens)->type != TOKEN_IDENTIFIER &&
            peek_token(tokens)->type != TOKEN_COMMA) {
            print_error("Invalid argument name: '%s'\n", (char*) peek_token(tokens)->val);
            exit(1);
        } else {
            if (peek_token(tokens)->type == TOKEN_COMMA &&
                peek_next_token(tokens)->type == TOKEN_RPAREN) {
                print_error("Argument list not terminated (trailing ',')\n");
                exit(1);
            }

            if (peek_token(tokens)->type == TOKEN_COMMA) {
                advance_token(tokens);
                continue;
            }

            if (peek_token(tokens)->type == TOKEN_IDENTIFIER) {
                argc++;
                args_list_node->symbols = realloc(args_list_node->symbols, sizeof(char*) * argc);
                if (args_list_node->symbols == NULL) {
                    print_error("Failed to re-allocate memory for argument list array\n");
                    exit(1);
                }

                args_list_node->symbols[argc - 1] = (char*) peek_token(tokens)->val;
                advance_token(tokens);
            }
        }
    }
    advance_token(tokens);
    if (peek_token(tokens)->type != TOKEN_LBRACE) {
        print_error("Function body opening brace '{' missing\n");
        exit(1);
    }
    advance_token(tokens);
    ExprNode* body = parse_expr(tokens);

    if (peek_token(tokens)->type != TOKEN_RBRACE) {
        print_error("Function body closing brace '}' missing\n");
        exit(1);
    }
    advance_token(tokens);

    FunctionDefNode* func_def_node = malloc(sizeof(FunctionDefNode));
    if (func_def_node == NULL) {
        print_error("Failed to allocate memory for FunctionDefNode\n");
        exit(1);
    }
    func_def_node->name = strdup(func_name);
    func_def_node->argc = argc;
    func_def_node->body = body;
    func_def_node->parameters = args_list_node;
    func_def_node->return_type = FUSION_TYPE_NULL;

    ExprNode* func_def_expr_node = malloc(sizeof(ExprNode));
    if (func_def_expr_node == NULL) {
        print_error("Failed to allocate memory for ExprNode\n");
        exit(1);
    }
    func_def_expr_node->type = EXPR_FUNC_DEF;
    func_def_expr_node->as.function_def_node = func_def_node;

    return func_def_expr_node;
}

ExprNode* parse_args_list_expr(TokenStream* tokens) { return NULL; }
