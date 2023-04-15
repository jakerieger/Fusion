// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#include "token_stream.h"
#include <malloc.h>

Token* peek_token(TokenStream* stream) { return &stream->tokens[stream->token_ptr]; }

Token* peek_next_token(TokenStream* stream) {
    if (&stream->tokens[stream->token_ptr + 1] != NULL) {
        return &stream->tokens[stream->token_ptr + 1];
    } else {
        return NULL;
    }
}
void advance_token(TokenStream* stream) { stream->token_ptr++; }

void cleanup_token_stream(TokenStream* stream) {
    free(stream->tokens);
    free(stream);
}
