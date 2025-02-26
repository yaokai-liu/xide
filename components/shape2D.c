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
 * Module Name: components
 * Filename: shape.c
 * Creator: Yaokai Liu
 * Create Date: 2024-7-9
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "shape2D.h"
#include "definition.h"
#include <math.h>

inline float SLine_length(const Line line) {
  uint32_t len_sq = (line[1].coord[AXIS_X] - line[0].coord[AXIS_X]) * (line[1].coord[AXIS_X] - line[0].coord[AXIS_X])
                    + (line[1].coord[AXIS_Y] - line[0].coord[AXIS_Y]) * (line[1].coord[AXIS_Y] - line[0].coord[AXIS_Y]);
  return sqrtf((float) len_sq);
}
