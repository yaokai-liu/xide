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
 * Filename: font-manage.c
 * Creator: Yaokai Liu
 * Create Date: 2025-02-22
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "font-manage.h"
#include "ft2build.h"
#include "minmax.h"
#include "object-enum.h"
#include "runtime.h"
#include "xgl-object.h"
#include FT_FREETYPE_H

typedef struct FontManager {
  const Allocator *allocator;
  FT_Library ftLibrary;
  Array /*<CharModelSet>*/ *setArray;
} FontManager;

void releaseCharModelSet(CharModelSet *set, const Allocator *) {
  FT_Done_Face(set->face);
  releasePrimeArray(set->modelArray);
  AVLTree_destroy(set->charTree, nullptr);
}

FontManager *FontManager_new(const Allocator *allocator) {
  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    rt_error("Could not init FreeType Library");
    return nullptr;
  }
  FontManager *manager = allocator->calloc(1, sizeof(FontManager));
  manager->allocator = allocator;
  manager->ftLibrary = ft;
  manager->setArray = Array_new(sizeof(CharModelSet), enum_IDE_CHAR_MODEL_SET, allocator);
  return manager;
}

void FontManager_destroy(FontManager *manager) {
  Array_reset(manager->setArray, (destruct_t *) releaseCharModelSet);
  FT_Done_FreeType(manager->ftLibrary);
  manager->allocator->free(manager);
}

CharModelSet *FontManager_loadFont(FontManager *manager, const Font *font) {
  CharModelSet *pSet = FontManager_findFont(manager, font);
  if (pSet) {
    rt_message("Loaded font");
    return pSet;
  }
  FT_Face face;
  FT_Error error = FT_New_Face(manager->ftLibrary, font->path, (FT_Long) font->index, &face);
  if (error != 0) {
    rt_error("Failed to load font");
    return nullptr;
  }
  FT_Set_Pixel_Sizes(face, 0, font->size);
  CharModelSet set = {
    .face = face, .font = {.path = font->path, .index = font->index, .size = font->size},
         .atlas = 0
  };
  set.modelArray = Array_new(sizeof(CharModel), enum_IDE_CHAR_MODEL, manager->allocator);
  set.charTree = AVLTree_new(manager->allocator, nullptr);
  Array_append(manager->setArray, &set, 1);
  return Array_last_real(manager->setArray);
}

CharModelSet *FontManager_findFont(FontManager *manager, const Font *font) {
  if (Array_length(manager->setArray) == 0) { return nullptr; }
  CharModelSet *first = Array_first_real(manager->setArray);
  CharModelSet *last = Array_last_real(manager->setArray);
  for (CharModelSet *set = first; set <= last; set++) {
    const Font *f = &set->font;
    if (font->size != f->size) { continue; }
    if (font->index != f->index) { continue; }
    if (strcmp(font->path, f->path) != 0) { continue; }
    return set;
  }
  return nullptr;
}

Array /*<Vertex2D>*/ *charModelSetGenHCoordArray(const CharModelSet *set, const Array /*<char_t>*/ *char_array,
                                                 const Vertex2D *anchor, float c_space, uint32_t mode,
                                                 XGLVector2D feedback_vec, const Allocator *allocator) {
  const uint32_t count = Array_length(char_array);
  if (!count) { return nullptr; }
  const char_t * const string = Array_real_addr(char_array, 0);
  Vertex2D *vertices = allocator->malloc(sizeof(Vertex2D) * count);
  float offset_x = 0, offset_y = 0;
  float origin = 0, height = 0, width = 0;
  for (uint32_t i = 0; i < count; i++) {
    const CharModel *model = AVLTree_get(set->charTree, string[i]);
    model = Array_virt2real(set->modelArray, model);
    offset_x = ((float) model->size[AXIS_X]) / 2 + ((float) model->bearing[AXIS_X]);
    offset_y = ((float) model->bearing[AXIS_Y]) - ((float) model->size[AXIS_Y]) / 2;
    vertices[i].coord[AXIS_X] = offset_x + origin;
    vertices[i].coord[AXIS_Y] = offset_y;
    vertices[i].color = anchor->color;
    height = max(height, (float) model->size[AXIS_Y]);
    origin += 1.0f / 64 * (float) (model->advance[AXIS_X]) + c_space;
  }
  width = origin;
  if (feedback_vec) {
    feedback_vec[AXIS_X] = width;
    feedback_vec[AXIS_Y] = height;
  }

  for (uint32_t i = 0; i < count; i++) {
    vertices[i].coord[AXIS_Y] = height - vertices[i].coord[AXIS_Y];
    vertices[i].coord[AXIS_X] += anchor->coord[AXIS_X];
    vertices[i].coord[AXIS_Y] += anchor->coord[AXIS_Y];
  }
  switch (mode & TS_H_MASK) {
    case TS_H_CENTER: {
      for (uint32_t i = 0; i < count; i++) { vertices[i].coord[AXIS_X] -= width / 2; }
      break;
    }
    case TS_LEFT: {
      for (uint32_t i = 0; i < count; i++) { vertices[i].coord[AXIS_X] -= width; }
      break;
    }
  }
  switch (mode & TS_V_MASK) {
    case TS_V_CENTER: {
      for (uint32_t i = 0; i < count; i++) { vertices[i].coord[AXIS_Y] -= height / 2; }
      break;
    }
    case TS_ABOVE: {
      for (uint32_t i = 0; i < count; i++) { vertices[i].coord[AXIS_Y] -= height; }
      break;
    }
  }
  Array *vertex_array = Array_new(sizeof(Vertex2D), enum_XGL_COORD, allocator);
  Array_append(vertex_array, vertices, count);
  allocator->free(vertices);
  return vertex_array;
}

Array /*<Vertex2D>*/ *charModelSetGenVCoordArray(const CharModelSet *set, const Array /*<char_t>*/ *char_array,
                                                 const Vertex2D *anchor, float c_space, uint32_t mode,
                                                 XGLVector2D feedback_vec, const Allocator *allocator) {
  return nullptr;
}
