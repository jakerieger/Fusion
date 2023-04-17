// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef FUSION_LEXER_H
#define FUSION_LEXER_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

typedef enum {
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_MODULO,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_COLON,
    TOKEN_COMMA,
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_EQUALS,
    TOKEN_BOOLEAN,
    TOKEN_KEYWORD,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    const char* start;
    int length;
    int line;
    void* val;
} Token;

static const char* source;
static const char* current;
static int line = 1;

void advance();

bool is_at_end();

char peek();

char peek_next();

bool match(char expected);

void skip_whitespace();

Token make_token(TokenType type, void* val);

Token error_token(const char* message);

Token number();

Token scan_token();

void init_scanner(const char* src);

#endif