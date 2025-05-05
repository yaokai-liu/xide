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
 * Filename: char-model-set.h
 * Creator: Yaokai Liu
 * Create Date: 2025-04-25
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_CHAR_MODEL_SET_H
#define XIDE_CHAR_MODEL_SET_H

#include "array.h"
#include "avl-tree.h"
#include "char_t.h"
#include "freetype/freetype.h"
#include "print.h"
#include <stdint.h>

typedef struct IDE IDE;

typedef struct CharModelSet {
  FT_Face face;
  Font font;
  uint32_t atlas;
  Array /*<CharModel>*/ *modelArray;
  AVLTree /*<uint64_t, uint64_t>*/ *charTree;
} CharModelSet;

const CharModelSet *ideUpdateCharModelSet(IDE *ide, const Font *font, const Array /*<char_t>*/ *char_array);

Array /*<Vertex2D>*/ *CharModelSet_genHCoordArray(const CharModelSet *set, const Array /*<char_t>*/ *char_array,
                                                  const Vertex2D *anchor, float c_space, uint32_t mode,
                                                  XGLVector2D feedback_vec, const Allocator *allocator);
Array /*<Vertex2D>*/ *CharModelSet_genVCoordArray(const CharModelSet *set, const Array /*<char_t>*/ *char_array,
                                                  const Vertex2D *anchor, float c_space, uint32_t mode,
                                                  XGLVector2D feedback_vec, const Allocator *allocator);


#endif  // XIDE_CHAR_MODEL_SET_H
