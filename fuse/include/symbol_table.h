// Copyright © 2023 Jake Rieger.
//
// Licensed under the GNU General Public License v3.0

#ifndef FUSION_SYMBOL_TABLE_H
#define FUSION_SYMBOL_TABLE_H

#include "hashmap.h"

typedef struct SymbolTableStack {
    HashMap** tables;
    int count;
} SymbolTableStack;

#endif // FUSION_SYMBOL_TABLE_H