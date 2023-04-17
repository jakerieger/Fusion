// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef FUSION_ERROR_H
#define FUSION_ERROR_H

#include "repl.h"

#define CHECK_MEM(__ptr) if (__ptr == NULL) { print_error("Memory allocation failed.\n"); exit(10); }

#endif
