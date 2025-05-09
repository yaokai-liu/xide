/**
 * xide - An integrated development environment
 * Copyright (C) 2024 Yaokai Liu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *
 * Project Name: xide
 * Module Name: computational-geometry
 * Filename: cg2d.h
 * Creator: Yaokai Liu
 * Create Date: 2024-11-21
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef COMPUTATION_GEOMETRY_2D_H
#define COMPUTATION_GEOMETRY_2D_H

#include "array.h"
#include <stdint.h>

Array *xglEarClippingTriangulate2D(const Array *vert_array, const Allocator *allocator);

Array *xglRadialTriangulation2D(const Array *vert_array, bool cycle, const Allocator *allocator);

#endif  // COMPUTATION_GEOMETRY_2D_H
