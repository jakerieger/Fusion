// Copyright © 2023 Jake Rieger.
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
    printf("%s(error) %s", BOLD_RED_COLOR, RED_COLOR);
    vfprintf(stdout, format, args);
    printf("%s\n", RESET_COLOR);
    va_end(args);
}