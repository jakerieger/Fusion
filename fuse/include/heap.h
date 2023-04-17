// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

#ifndef FUSION_HEAP_H
#define FUSION_HEAP_H

#include <stdlib.h>

typedef struct {
    size_t size;
    void* entry_ptr;
    void* end_ptr;
} Heap;

Heap* create_heap(size_t size);
Heap* resize_heap(Heap* current, size_t new_size);
int cleanup_heap(Heap* heap);

#endif