/* License
 *
 * xide - An integrated development environment
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
 * Filename: print.h
 * Creator: Yaokai Liu
 * Create Date: 2025-02-22
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_PRINT_H
#define XIDE_PRINT_H

#include "char_t.h"
#include "runtime-enum.h"
#include "shape2d.h"
#include "texture-manage.h"
#include "xgl-object.h"

enum TEXT_STYLE {
  TS_RIGHT = 0b00,
  TS_LEFT = 0b01,
  TS_H_CENTER = 0b10,
  TS_H_MASK = 0b11,

  TS_V_CENTER = 0b0000,
  TS_ABOVE = 0b0100,
  TS_BELOW = 0b1000,
  TS_V_MASK = 0b1100,

  TS_HORIZONTAL = 0x00,
  TS_VERTICAL = 0x10,
  TS_D_MASK = 0xF0,
};

typedef struct Font {
  const char_t *path;  // path to font file
  uint64_t index;
  uint32_t size;
} Font;

typedef struct CharModel {
  uint32_t code;
  uint32_t offset;
  uint32_t size[2];
  int64_t advance[2];
  int32_t bearing[2];
} CharModel;

typedef XGLRgba fn_pcolor_t(uint32_t pos[2]);
typedef XGLRgba fn_color_t(float pos[2]);

void xglGenCharCoord2D(const CharModel *model, const Vertex2D *center, const TextureAtlas *atlas, XGLVertex dest[4]);

#endif  // XIDE_PRINT_H
