/**
 * Project Name: xide
 * Module Name: components
 * Filename: shape.h
 * Creator: Yaokai Liu
 * Create Date: 2024-7-9
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_SHAPE2D_H
#define XIDE_SHAPE2D_H

#include "array.h"

typedef struct Vertex {
  uint32_t coord[2];
  uint32_t color;
} Vertex;

typedef struct Line {
  uint32_t start[2];
  uint32_t end[2];
  uint32_t colors[2];
} Line;

#endif  // XIDE_SHAPE2D_H
