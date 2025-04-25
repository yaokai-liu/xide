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
 * Filename: char-model-set.c
 * Creator: Yaokai Liu
 * Create Date: 2025-04-25
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "char-model-set.h"
#include "minmax.h"
#include "runtime-msg.h"
#include "ide.h"

CharModelSet *ideGenCharModelSet(IDE *ide, const Font *font);
uint32_t ideUpdateCharacterTextureAtlas(IDE *ide, const Array /*<char_t>*/ *char_array, CharModelSet *set);

const CharModelSet *ideUpdateCharModelSet(IDE *ide, const Font *font, const Array /*<char_t>*/ *char_array) {
  CharModelSet *set = ideGenCharModelSet(ide, font);
  if (!set) { return nullptr; }
  ideUpdateCharacterTextureAtlas(ide, char_array, set);
  return set;
}


CharModelSet *ideGenCharModelSet(IDE *ide, const Font *font) {
  CharModelSet *set = FontManager_loadFont(ide->fontManager, font);
  if (!set) { return nullptr; }
  set = FontManager_realCharModelSet(ide->fontManager, set);
  if (set->atlas) { return set; }
  TextureAtlas atlas = {.unit = 0, .texture = 0, .width = 0, .height = 0};
  Array_append(ide->atlasManager, &atlas, 1);
  set->atlas = Array_length(ide->atlasManager);
  return set;
}

uint32_t ideUpdateCharacterTextureAtlas(IDE *ide, const Array /*<char_t>*/ *char_array, CharModelSet *set) {
  const Allocator *allocator = ide->allocator;
  TextureAtlas *atlas = Array_real_addr(ide->atlasManager, set->atlas - 1);
  const uint32_t old_width = atlas->width, old_height = atlas->height;
  uint32_t current_width = atlas->width + 1, current_height = atlas->height;
  Array *update_array = Array_new(sizeof(char_t), enum_IDE_CHAR, allocator);
  const char_t *character = Array_first_real(char_array);
  const char_t *last = Array_last_real(char_array);
  bool has_update = false;
  for (; character <= last; character++) {
    if (AVLTree_get(set->charTree, *character)) { continue; }
    has_update = true;
    FT_Error err = FT_Load_Char(set->face, *character, FT_LOAD_RENDER);
    if (err) {
      rt_error("Loading glyph of '%c' in %s failed", *character, set->font.path);
      continue;
    }
    const uint32_t sub_width = set->face->glyph->bitmap.width + 1;
    const uint32_t sub_height = set->face->glyph->bitmap.rows;
    const int32_t bearing_x = set->face->glyph->bitmap_left;
    const int32_t bearing_y = set->face->glyph->bitmap_top;
    const int64_t advance_x = set->face->glyph->advance.x;
    const int64_t advance_y = set->face->glyph->advance.y;
    const CharModel model = {
      .code = *character,
      .offset = current_width,
      .size = {[AXIS_X] = sub_width, [AXIS_Y] = sub_height},
      .bearing = {[AXIS_X] = bearing_x, [AXIS_Y] = bearing_y },
      .advance = {[AXIS_X] = advance_x, [AXIS_Y] = advance_y }
    };
    Array_append(set->modelArray, &model, 1);
    Array_append(update_array, character, 1);
    REFER(CharModel) v_model = Array_last_virt(set->modelArray);
    AVLTree_set(set->charTree, *character, v_model);
    current_height = max(current_height, sub_height + 1);
    current_width += sub_width + 2;
  }
  if (!has_update) { return 0; }
  atlas->width = current_width;
  atlas->height = current_height;
  if (Array_length(update_array) == 0) {
    releasePrimeArray(update_array);
    return 0;
  }
  const uint32_t old_texture = atlas->texture;
  glCreateTextures(GL_TEXTURE_2D, 1, &atlas->texture);
  glTextureStorage2D(atlas->texture, 1, GL_R8, (GLsizei) current_width, (GLsizei) current_height);
  if (old_texture) {
    glCopyImageSubData(old_texture, GL_TEXTURE_2D, 0, 0, 0, 0, atlas->texture, GL_TEXTURE_2D, 0, 0, 0, 0,
                       (GLsizei) old_width - 1, (GLsizei) old_height, 0);
    glDeleteTextures(1, &old_texture);
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  character = Array_first_real(update_array);
  last = Array_last_real(update_array);
  for (; character <= last; character++) {
    const CharModel *model = AVLTree_get(set->charTree, *character);
    model = Array_virt2real(set->modelArray, model);
    FT_Load_Char(set->face, *character, FT_LOAD_RENDER);
    const uint32_t offset = model->offset;
    const uint8_t *data = set->face->glyph->bitmap.buffer;
    glTextureSubImage2D(atlas->texture, 0, (GLint) offset, 0, (GLsizei) model->size[AXIS_X] - 1,
                        (GLsizei) model->size[AXIS_Y], GL_RED, GL_UNSIGNED_BYTE, data);
  }
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

  glTextureParameteri(atlas->texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTextureParameteri(atlas->texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTextureParameteri(atlas->texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(atlas->texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  uint32_t count = Array_length(update_array);
  releasePrimeArray(update_array);
  return count;
}


Array /*<Vertex2D>*/ *CharModelSet_genHCoordArray(const CharModelSet *set, const Array /*<char_t>*/ *char_array,
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

Array /*<Vertex2D>*/ *CharModelSet_genVCoordArray(const CharModelSet *set, const Array /*<char_t>*/ *char_array,
                                                  const Vertex2D *anchor, float c_space, uint32_t mode,
                                                  XGLVector2D feedback_vec, const Allocator *allocator) {
  return nullptr;
}
