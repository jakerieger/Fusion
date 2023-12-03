// Copyright © 2023 Jake Rieger.
//
// Licensed under the GNU General Public License v3.0

#ifndef FUSION_ERROR_H
#define FUSION_ERROR_H

#include "repl.h"

#define CHECK_MALLOC(__ptr, __name) if (__ptr == NULL) { print_error("Memory allocation for '%s' failed.\n", __name); exit(10); }
#define CHECK_REALLOC(__ptr, __name) if (__ptr == NULL) { print_error("Memory re-allocation for '%s' failed.\n", __name); exit(10); }
#define CHECK_NULL_FATAL(__ptr, __name) if (__ptr == NULL) { print_error("'%s' is nullptr.\n", __name); exit(10); }

#endif
