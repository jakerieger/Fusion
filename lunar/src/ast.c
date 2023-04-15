// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#include "ast.h"
#include <malloc.h>
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
    bool_node->value = *(LunaBoolean*) token->val;

    ExprNode* bool_expr_node = malloc(sizeof(ExprNode));
    bool_expr_node->type = EXPR_BOOLEAN;
    bool_expr_node->as.boolean_node = bool_node;

    return bool_expr_node;
}
