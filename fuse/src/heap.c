// Copyright © 2023 Jake Rieger.
//
// Licensed under the GNU General Public License v3.0

#include "heap.h"

Heap* create_heap(size_t size) {
    Heap* heap;
    heap = (Heap*) malloc(size);
    if (heap == NULL) return NULL;
    heap->size = size;
    heap->entry_ptr = (void*) heap;
    heap->end_ptr = (void*) &heap[size - 1];

    return heap;
}

Heap* resize_heap(Heap* current, size_t new_size) { return realloc(current, new_size); }

int cleanup_heap(Heap* heap) {
    free(heap);
    return 0;
}
