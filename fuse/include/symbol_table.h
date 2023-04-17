// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef FUSION_SYMBOL_TABLE_H
#define FUSION_SYMBOL_TABLE_H

typedef struct SymbolTable {

} SymbolTable;

typedef struct SymbolTableStack {
    SymbolTable** tables;
    int count;
} SymbolTableStack;

#endif // FUSION_SYMBOL_TABLE_H