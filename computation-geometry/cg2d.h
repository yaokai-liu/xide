/**
 * Project Name: xide
 * Module Name: computation-geometry
 * Filename: com-geo.h
 * Creator: Yaokai Liu
 * Create Date: 2024-11-21
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef COMPUTATION_GEOMETRY_2D_H
#define COMPUTATION_GEOMETRY_2D_H

#include "array.h"
#include <stdint.h>

Array *xglCreateDelaunayIndexArray(Array *vertex_array, const Allocator *allocator);

#endif  // COMPUTATION_GEOMETRY_2D_H
