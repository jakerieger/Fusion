// Copyright © 2023 Jake Rieger.
//
// Licensed under the GNU General Public License v3.0

#ifndef FUSION_COLORS_H
#define FUSION_COLORS_H

#include <stdarg.h>
#include <stdio.h>

#define RESET_COLOR "\033[0m"
#define BLACK_COLOR "\033[30m"
#define RED_COLOR "\033[31m"
#define GREEN_COLOR "\033[32m"
#define YELLOW_COLOR "\033[33m"
#define BLUE_COLOR "\033[34m"
#define MAGENTA_COLOR "\033[35m"
#define CYAN_COLOR "\033[36m"
#define WHITE_COLOR "\033[37m"
#define BOLD_BLACK_COLOR "\033[1m\033[30m"
#define BOLD_RED_COLOR "\033[1m\033[31m"
#define BOLD_GREEN_COLOR "\033[1m\033[32m"
#define BOLD_YELLOW_COLOR "\033[1m\033[33m"
#define BOLD_BLUE_COLOR "\033[1m\033[34m"
#define BOLD_MAGENTA_COLOR "\033[1m\033[35m"
#define BOLD_CYAN_COLOR "\033[1m\033[36m"
#define BOLD_WHITE_COLOR "\033[1m\033[37m"
#define BG_BLACK_COLOR "\033[40m"
#define BG_RED_COLOR "\033[41m"
#define BG_GREEN_COLOR "\033[42m"
#define BG_YELLOW_COLOR "\033[43m"
#define BG_BLUE_COLOR "\033[44m"
#define BG_MAGENTA_COLOR "\033[45m"
#define BG_CYAN_COLOR "\033[46m"
#define BG_WHITE_COLOR "\033[47m"

void print_colored(const char* restrict color_code, const char* restrict format, ...);
void print_error(const char* restrict format, ...);

#endif