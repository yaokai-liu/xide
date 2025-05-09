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
 * Module Name: widgets
 * Filename: shape2d.h
 * Creator: Yaokai Liu
 * Create Date: 2024-7-9
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_SHAPE2D_H
#define XIDE_SHAPE2D_H

#include "array.h"

typedef struct PixelVertex2D {
  uint32_t coord[2];
  uint32_t color;
} PixelVertex2D;

typedef struct Vertex2D {
  float coord[2];
  uint32_t color;
} Vertex2D;

typedef PixelVertex2D Line[2];
float SLine_length(const Line line);

#endif  // XIDE_SHAPE2D_H
