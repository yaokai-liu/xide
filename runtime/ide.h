/* License
 *
 * ${PROJ_DESCRIPTION}
 * Copyright (C) 2025 Yaokai Liu
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
 * Module Name: runtime
 * Filename: ide.h
 * Creator: Yaokai Liu
 * Create Date: 2025-02-22
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_IDE_H
#define XIDE_IDE_H

#include "array.h"
#include "draw.h"
#include "print.h"

typedef struct IDE IDE;

IDE *IDE_new(const Allocator *allocator);

DrawTask *ideCreatePrint2D(IDE *ide, Array /*<char_t>*/ *char_array,
                           Array /*<PixelVertex2D>*/ *vertex_array, uint32_t plane_index,
                           Font *font);
#endif  // XIDE_IDE_H
