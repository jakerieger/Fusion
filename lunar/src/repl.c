// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#include "repl.h"
#include <stdio.h>

void print_colored(const char* restrict color_code, const char* restrict format, ...) {
    va_list args;
    va_start(args, format);
    printf("%s", color_code);
    vfprintf(stdout, format, args);
    printf("%s", RESET_COLOR);
    va_end(args);
}

void print_error(const char* restrict format, ...) {
    va_list args;
    va_start(args, format);
    printf("%sError: %s", BOLD_RED_COLOR, BOLD_WHITE_COLOR);
    vfprintf(stderr, format, args);
    printf("%s", RESET_COLOR);
    va_end(args);
}