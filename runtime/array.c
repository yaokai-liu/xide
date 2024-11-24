/**
 * Project Name: xide
 * Module Name: runtime
 * Filename: array.c
 * Creator: Yaokai Liu
 * Create Date: 2024-7-3
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "array.h"
#include "allocator.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct Array {
  const Allocator *allocator;
  uint32_t alloc_len;
  uint32_t ele_size;
  uint32_t used_len;
  void *elements;
};

const size_t sizeof_array = sizeof(Array);

#define ALLOC_LEN 32

Array *Array_new(const uint32_t ele_size, const Allocator * const allocator) {
  Array *array = allocator->calloc(1, sizeof(struct Array));
  array->allocator = allocator;
  if (ele_size == 0) { return nullptr; }
  Array_init(array, ele_size);
  return array;
}

uint32_t Array_init(Array *array, const uint32_t ele_size) {
  array->ele_size = ele_size;
  array->elements = nullptr;
  array->alloc_len = 0;
  array->used_len = 0;
  return ele_size;
}

inline uint32_t Array_length(const Array *array) {
  return array->used_len;
}

inline void *Array_get(const Array *array, uint32_t index) {
  if (index >= array->used_len) { return nullptr; }
  return (char *) array->elements + array->ele_size * index;
}

uint32_t Array_append(Array *array, const void *elements, const uint32_t count) {
  if (array->used_len + count >= array->alloc_len) {
    uint32_t length = ((array->used_len + count) / ALLOC_LEN + 1) * ALLOC_LEN;
    void *p = array->allocator->realloc(array->elements, length * array->ele_size);
    if (!p) { return -1; }
    array->elements = p;
    array->alloc_len = length;
  }
  void *dest = (char *) array->elements + array->ele_size * array->used_len;
  memcpy(dest, elements, count * array->ele_size);
  array->used_len += count;
  return count;
}

bool Array_any(const Array *array, bool (*fn_judgment)(void *)) {
  bool judge = false;
  for (uint32_t i = 0; i < array->used_len; i++) {
    void *element = (char *) array->elements + i * array->ele_size;
    if (fn_judgment(element)) { judge = true; }
  }
  return judge;
}

bool Array_all(const Array *array, bool (*fn_judgment)(void *)) {
  bool judge = true;
  for (uint32_t i = 0; i < array->used_len; i++) {
    void *element = (char *) array->elements + i * array->ele_size;
    if (!fn_judgment(element)) { judge = false; }
  }
  return judge;
}

Array *Array_filter(const Array *origin_array, bool (*fn_judgment)(const void *)) {
  Array *filtered_array = Array_new(origin_array->ele_size, origin_array->allocator);
  for (int i = 0; i < Array_length(origin_array); i++) {
    const void *ele = Array_get(origin_array, i);
    if (fn_judgment(ele)) { Array_append(filtered_array, ele, 1); }
  }
  return filtered_array;
}

Array *Array_deduplicate(const Array *origin_array, bool (*fn_equal)(const void *, const void *)) {
  Array *filtered_array = Array_new(origin_array->ele_size, origin_array->allocator);
  for (int i = 0; i < Array_length(origin_array); i++) {
    const void *ele1 = Array_get(origin_array, i);
    for (uint32_t j = 0; j < Array_length(filtered_array); j++) {
      const void *ele2 = Array_get(origin_array, j);
      if (fn_equal(ele1, ele2)) { goto __deduplicate_find_duplicated; }
    }
    Array_append(filtered_array, ele1, 1);
__deduplicate_find_duplicated:;
  }
  return filtered_array;
}

uint32_t Array_clear(Array *array, void (*fn_free)(void *, const Allocator *)) {
  if (fn_free) {
    for (uint32_t i = 0; i < array->used_len; i++) {
      void *ele = Array_get(array, i);
      fn_free(ele, array->allocator);
    }
  }
  const uint32_t len = array->used_len;
  array->used_len = 0;
  return len;
}

uint32_t Array_reset(Array *array, void (*fn_free)(void *, const Allocator *)) {
  Array_clear(array, fn_free);
  const uint32_t len = array->alloc_len;
  if (array->elements) { array->allocator->free(array->elements); }
  array->elements = nullptr;
  array->alloc_len = 0;
  array->used_len = 0;
  return len;
}

void Array_destroy(Array *array) {
  array->allocator->free(array);
}
