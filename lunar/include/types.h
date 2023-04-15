// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef LUNA_TYPES_H
#define LUNA_TYPES_H

#define BOOL_TRUE 0x1
#define BOOL_FALSE 0x0

typedef double LunaNumber;
typedef char* LunaString;
#pragma pack(1)
typedef struct LunaBoolean {
    unsigned value : 1;
} LunaBoolean;
#pragma pack(0)

typedef enum LunaType {
    LUNA_TYPE_BOOLEAN,
    LUNA_TYPE_NUMBER,
    LUNA_TYPE_STRING,
    LUNA_TYPE_NULL
} LunaType;

#endif  //LUNA_TYPES_H
