// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef LUNA_TYPES_H
#define LUNA_TYPES_H

#define BOOL_TRUE 0x1
#define BOOL_FALSE 0x0

typedef double Number;
typedef char* String;
#pragma pack(1)
typedef struct Boolean {
    unsigned value : 1;
} Boolean;
#pragma pack(0)

#endif  //LUNA_TYPES_H
