/**
 * Project Name: xide
 * Module Name: com-geo
 * Filename: com-geo.h
 * Creator: Yaokai Liu
 * Create Date: 2024-11-21
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef COMPUTATION_GEOMETRY_2D_H
#define COMPUTATION_GEOMETRY_2D_H

#include "array.h"
#include <stdint.h>

Array *xglConvexTriangulate(Array *vertex_array, const Allocator *allocator);
Array *xglEarClippingTriangulate(const Array *vert_array, const Allocator *allocator);
#endif  // COMPUTATION_GEOMETRY_2D_H
