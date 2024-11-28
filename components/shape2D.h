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

typedef struct PixelVertex {
  uint32_t coord[2];
  uint32_t color;
} PixelVertex;

typedef struct Vertex {
  float coord[2];
  uint32_t color;
} Vertex;

typedef PixelVertex Line[2];
float SLine_length(const Line line);

#endif  // XIDE_SHAPE2D_H
