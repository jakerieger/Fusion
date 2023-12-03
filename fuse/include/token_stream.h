// Copyright © 2023 Jake Rieger.
//
// Licensed under the GNU General Public License v3.0

#ifndef FUSION_TOKEN_STREAM_H
#define FUSION_TOKEN_STREAM_H

#include "lexer.h"
typedef struct TokenStream {
    Token* tokens;
    size_t count;
    int token_ptr;
} TokenStream;

Token* peek_token(TokenStream* stream);
Token* peek_next_token(TokenStream* stream);
void advance_token(TokenStream* stream);
void cleanup_token_stream(TokenStream* stream);

#endif
