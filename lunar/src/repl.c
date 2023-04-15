// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#include <stdio.h>
/*
 Copyright (c) 2023 Gymate, Inc.

 Licensed under the GNU General Public License v3.0
 */

#include "repl.h"

void print_colored(const char* restrict color_code, const char* restrict string) {
    printf("%s%s%s", color_code, string, RESET_COLOR);
}

void print_error(const char* restrict msg) {
    printf("%s⚠ %s%s\n", BOLD_RED_COLOR, msg, RESET_COLOR);
}