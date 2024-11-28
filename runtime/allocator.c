/**
 * Project Name: xide
 * Module Name: runtime
 * Filename: allocator.c
 * Creator: Yaokai Liu
 * Create Date: 2024-7-6
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "allocator.h"
#include <malloc.h>

const Allocator STDAllocator = {
  .malloc = malloc, .realloc = realloc, .calloc = calloc, .free = free};
