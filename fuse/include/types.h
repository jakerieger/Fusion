// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef FUSION_TYPES_H
#define FUSION_TYPES_H

#define BOOL_TRUE 0x1
#define BOOL_FALSE 0x0

typedef double FusionNumber;
typedef char* FusionString;
#pragma pack(1)
typedef struct FusionBoolean {
    unsigned value : 1;
} FusionBoolean;
#pragma pack(0)

typedef enum FusionType {
    FUSION_TYPE_BOOLEAN,
    FUSION_TYPE_NUMBER,
    FUSION_TYPE_STRING,
    FUSION_TYPE_NULL
} FusionType;

#endif  //FUSION_TYPES_H