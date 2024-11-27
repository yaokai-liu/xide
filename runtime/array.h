/**
 * Project Name: xide
 * Module Name: runtime
 * Filename: array.h
 * Creator: Yaokai Liu
 * Create Date: 2024-7-3
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_ARRAY_H
#define XIDE_ARRAY_H

#include "allocator.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct Array Array;
extern const size_t sizeof_array;

Array *Array_new(uint32_t ele_size, const Allocator *allocator);

uint32_t Array_init(Array *array, const uint32_t ele_size, const Allocator *allocator);

uint32_t Array_length(const struct Array *array);

// Note: append may change elements' address,
// so it is not promised that two `Array_get` of one same `index` will return a
// same address.
void *Array_get(const struct Array *array, uint32_t index);
uint32_t Array_append(struct Array *array, const void *elements, uint32_t count);

// Promised that every element would be detected with `fn_judgment`.
// So that for traversing elements.
bool Array_any(const struct Array *array, bool (*fn_judgment)(void *));
// Promised that every element would be detected with `fn_judgment`.
// So that for traversing elements.
bool Array_all(const struct Array *array, bool (*fn_judgment)(void *));

// Suppose `_to` and `_from` both are not duplicated array.
uint32_t Array_no_duplicated_concat(struct Array * restrict _to,
                                    const struct Array * restrict _from);
// Filter an array by fn_judgment. The origin_array will not be clean and destroy.
Array *Array_filter(const Array *origin_array, bool (*fn_judgment)(const void *));
// Deduplicate an array by fn_equal. The origin_array will not be clean and destroy.
Array *Array_deduplicate(const Array *origin_array, bool (*fn_equal)(const void *, const void *));
// Clear array and free all element with `fn_free`.
uint32_t Array_clear(struct Array *array, void (*fn_free)(void *, const Allocator *));
// Reset array and free all element with `fn_free`.
uint32_t Array_reset(struct Array *array, void (*fn_free)(void *, const Allocator *));
// Maybe cause memory leak if not reset array before destroy it.
void Array_destroy(struct Array *array);

#endif  // XIDE_ARRAY_H