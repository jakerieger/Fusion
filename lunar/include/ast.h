// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef LUNA_AST_H
#define LUNA_AST_H

#include "token_stream.h"

/**
 * @brief The base AST node used to represent an expression.
 */
typedef struct ExprNode {
    enum {
        EXPR_ADD,
        EXPR_SUBTRACT,
        EXPR_MULTIPLY,
        EXPR_DIVIDE,
        EXPR_MODULUS,
        EXPR_NUMBER,
        EXPR_STR,
        EXPR_REFERENCE,
        EXPR_ASSIGN,
        EXPR_FUNC_CALL,
    } type;
    union {
        struct BinaryOpNode* binary_op_node;
        struct NumberNode* number_node;
        struct StringNode* string_node;
        struct ReferenceNode* reference_node;
        struct AssignmentOpNode* assign_op_node;
        struct FunctionCallNode* function_call_node;
    } as;
} ExprNode;

/**
 * @brief AST node used to represent a binary operation such as addition or subtraction.
 */
typedef struct BinaryOpNode {
    ExprNode* left;
    ExprNode* right;
} BinaryOpNode;

typedef struct AssignmentOpNode {
    bool is_reference;
    union {
        struct ExprNode* expr_node;
        struct ReferenceNode* reference_node;
    } left;
    struct ExprNode* right;
} AssignmentOpNode;

/**
 * @brief AST node used to represent a number value.
 */
typedef struct NumberNode {
    double value;
} NumberNode;

typedef struct StringNode {
    char* value;
} StringNode;

/**
 * @brief AST node used to represent a variable
 */
typedef struct ReferenceNode {
    char* name;
    double value;
} ReferenceNode;

/**
 * @brief AST node used to represent a function call
 */
typedef struct FunctionCallNode {
    char* name;
    int argc;
    ExprNode* args;
} FunctionCallNode;

/**
 * @brief Parses a stream of tokens in to an AST tree and returns the root node
 * @param tokens
 * @return Pointer to parsed ExprNode
 */
ExprNode* parse_expr(TokenStream* tokens);

/**
 * @brief Parses a stream of tokens in to an `EXPR_ADD` or `EXPR_SUBTRACT` expression
 * @param tokens
 * @return Pointer to parsed ExprNode
 */
ExprNode* parse_add_expr(TokenStream* tokens);

/**
 * @brief Parses a stream of tokens in to a `EXPR_MULTIPLY`, `EXPR_DIVIDE`, or `EXPR_MODULUS` expression
 * @param tokens
 * @return Pointer to parsed ExprNode
 */
ExprNode* parse_mul_expr(TokenStream* tokens);

/**
 * @brief Parses a stream of tokens in to an assignment expression
 * @param tokens
 * @return Pointer to parsed ExprNode
 */
ExprNode* parse_assign_expr(TokenStream* tokens);

/**
 * @brief Parses a stream of tokens in to an identifier expression
 * @param tokens
 * @return Pointer to parsed ExprNode
 */
ExprNode* parse_identifier_expr(TokenStream* tokens);

/**
 * @brief Parses a stream of tokens in to a primary expression tree, such as a function call
 * @param tokens
 * @return Pointer to parsed ExprNode
 */
ExprNode* parse_primary_expr(TokenStream* tokens);

/**
 * @brief Parses a stream of tokens in to a number expression
 * @param tokens
 * @return Pointer to parsed ExprNode
 */
ExprNode* parse_number_expr(TokenStream* tokens);

/**
 * @brief Parses a stream of tokens in to a string expression
 * @param tokens
 * @return Pointer to parsed ExprNode
 */
ExprNode* parse_str_expr(TokenStream* tokens);

#endif
