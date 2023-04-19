// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#include "lexer.h"
#include "repl.h"
#include "types.h"
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR_LEN 64

void advance() { current++; }

bool is_at_end() { return *current == '\0'; }

char peek() { return *current; }

char peek_next() {
    if (is_at_end()) return '\0';
    return *(current + 1);
}

bool match(char expected) {
    if (is_at_end() || *current != expected) { return false; }
    current++;
    return true;
}

void skip_whitespace() {
    while (isspace(*current)) {
        if (*current == '\n') { line++; }
        current++;
    }
}

Token make_token(TokenType type, void* val) {
    Token token;
    token.type = type;
    token.start = current;
    token.length = 1;
    token.line = line;
    token.val = val;

    return token;
}

Token error_token(const char* message) {
    Token token;
    token.type = TOKEN_EOF;
    token.start = message;
    token.length = strlen(message);
    token.line = line;
    return token;
}

Token number() {
    char buf[64] = {'\0'};
    int len = 0;
    while (isdigit(peek())) {
        buf[len] = peek();
        advance();
        len++;
    }

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peek_next())) {
        advance();

        while (isdigit(peek())) advance();
    }

    char* number_value = malloc((len + 1) * sizeof(char));
    if (number_value == NULL) { print_error("Failed to allocate memory for integer conversion."); }

    strncpy(number_value, buf, len);
    number_value[len] = '\0';

    double* i = malloc(sizeof(double));
    *i = floor(atoi(number_value));
    return make_token(TOKEN_NUMBER, i);
}

Token identifier() {
    char buf[64] = {'\0'};
    int len = 0;
    while (isalpha(peek())) {
        buf[len] = peek();
        advance();
        len++;
    }

    char* identifier_name = malloc((len + 1) * sizeof(char));
    if (identifier_name == NULL) { print_error("Failed to allocate memory for identifier name."); }

    strncpy(identifier_name, buf, len);
    identifier_name[len] = '\0';

    if (strcmp(identifier_name, "true") == 0) {
        FusionBoolean* value = malloc(sizeof(FusionBoolean));
        value->value = BOOL_TRUE;
        return make_token(TOKEN_BOOLEAN, value);
    }

    if (strcmp(identifier_name, "false") == 0) {
        FusionBoolean* value = malloc(sizeof(FusionBoolean));
        value->value = BOOL_FALSE;
        return make_token(TOKEN_BOOLEAN, value);
    }

    // Lex keywords
    if (strcmp(identifier_name, "impl") == 0) { return make_token(TOKEN_KEYWORD, "impl"); }
    if (strcmp(identifier_name, "if") == 0) { return make_token(TOKEN_KEYWORD, "if"); }
    if (strcmp(identifier_name, "else") == 0) { return make_token(TOKEN_KEYWORD, "else"); }
    if (strcmp(identifier_name, "while") == 0) { return make_token(TOKEN_KEYWORD, "while"); }
    if (strcmp(identifier_name, "for") == 0) { return make_token(TOKEN_KEYWORD, "for"); }

    return make_token(TOKEN_IDENTIFIER, identifier_name);
}

Token scan_token() {
    skip_whitespace();

    if (is_at_end()) { return make_token(TOKEN_EOF, 0); }

    char c = peek();
    switch (c) {
        case '+': {
            advance();
            return make_token(TOKEN_PLUS, "+");
        }
        case '-': {
            advance();
            return make_token(TOKEN_MINUS, "-");
        }
        case '*': {
            advance();
            return make_token(TOKEN_MULTIPLY, "*");
        }
        case '/': {
            advance();
            return make_token(TOKEN_DIVIDE, "/");
        }
        case '%': {
            advance();
            return make_token(TOKEN_MODULO, "%");
        }
        case '(': {
            advance();
            return make_token(TOKEN_LPAREN, "(");
        }
        case ')': {
            advance();
            return make_token(TOKEN_RPAREN, ")");
        }
        case '=': {
            advance();
            return make_token(TOKEN_EQUALS, "=");
        }
        case ':': {
            advance();
            return make_token(TOKEN_COLON, ":");
        }
        case ',': {
            advance();
            return make_token(TOKEN_COMMA, ",");
        }
        case '{': {
            advance();
            return make_token(TOKEN_LBRACE, "{");
        }
        case '}': {
            advance();
            return make_token(TOKEN_RBRACE, "}");
        }
        case ';': {
            advance();
            return make_token(TOKEN_SEMICOLON, ";");
        }
        case '"': {
            advance();
            char str[MAX_STR_LEN] = {'\0'};
            int len = 0;
            while (!is_at_end() && peek() != '"' && len < MAX_STR_LEN) {
                if (peek() == '\\') {
                    advance();
                    switch (peek()) {
                        case 'n':
                            str[len++] = '\n';
                            break;
                        case 't':
                            str[len++] = '\t';
                            break;
                        case '"':
                            str[len++] = '\"';
                            break;
                        case '\\':
                            str[len++] = '\\';
                            break;
                        default: {
                            print_error("Invalid escape character.");
                            return error_token("Invalid escape character.");
                        }
                    }
                } else {
                    str[len++] = peek();
                }
                advance();
            }

            if (is_at_end()) {
                print_error("Unterminated string.");
                return error_token("Unterminated string.");
            } else {
                advance();
            }

            return make_token(TOKEN_STRING, strdup(str));
        }
        default: {
            if (isdigit(c)) return number();
            if (isalpha(c)) return identifier();

            break;
        }
    }

    print_error("Unexpected character.");

    if (peek() == '"') { print_error("Did you forget a beginning quote?"); }

    return error_token("Unexpected character.");
}

void init_scanner(const char* src) {
    source = src;
    current = source;
    line = 1;
}