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
 * Filename: font-manage.h
 * Creator: Yaokai Liu
 * Create Date: 2025-02-22
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_FONT_MANAGE_H
#define XIDE_FONT_MANAGE_H

#include "array.h"
#include "avl-tree.h"
#include "char_t.h"
#include "freetype/freetype.h"
#include "print.h"
#include <stdint.h>

typedef struct CharModelSet {
  FT_Face face;
  Font font;
  uint32_t atlas;
  Array /*<CharModel>*/ *modelArray;
  AVLTree /*<uint64_t, uint64_t>*/ *charTree;
} CharModelSet;

typedef struct FontManager FontManager;

FontManager *FontManager_new(const Allocator *allocator);
void FontManager_destroy(FontManager *manager);
CharModelSet *FontManager_loadFont(FontManager *manager, const Font *font);
CharModelSet *FontManager_findFont(FontManager *manager, const Font *font);
Array/*<Vertex2D>*/ *
charModelSetGenHCoordArray(const CharModelSet *set, const Array /*<char_t>*/ *char_array,
                           const Vertex2D * anchor, int32_t c_space, uint32_t mode,
                           XGLVector2D feedback_vec, const Allocator *allocator);
Array/*<Vertex2D>*/ *
charModelSetGenVCoordArray(const CharModelSet *set, const Array /*<char_t>*/ *char_array,
                           const Vertex2D * anchor, int32_t c_space, uint32_t mode,
                           XGLVector2D feedback_vec, const Allocator *allocator);

#endif  // XIDE_FONT_MANAGE_H
