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
 * Filename: ide.c
 * Creator: Yaokai Liu
 * Create Date: 2025-02-22
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "ide.h"
#include "enum.h"
#include "font-manage.h"
#include "print.h"
#include "runtime.h"
#include "texture-manage.h"
#include "utils.h"
#include <minmax.h>

typedef struct IDE {
  const Allocator *allocator;
  FontManager *fontManager;
  TextureAtlasManager *atlasManager;
} IDE;

IDE *IDE_new(const Allocator *allocator) {
  IDE *ide = allocator->calloc(1, sizeof(IDE));
  ide->allocator = allocator;
  ide->atlasManager = Array_new(sizeof(TextureAtlas), enum_IDE_TEXTURE_ATLAS, allocator);
  ide->fontManager = FontManager_new(allocator);
  return ide;
}

CharModelSet *ideGenCharModelSet(IDE *ide, Font *font) {
  CharModelSet *set = FontManager_loadFont(ide->fontManager, font);
  if (!set) { return nullptr; }
  if (set->atlas) { return set; }
  TextureAtlas atlas = {.unit = 0, .texture = 0, .width = 0, .height = 0};
  Array_append(ide->atlasManager, &atlas, 1);
  set->atlas = Array_length(ide->atlasManager);
  return set;
}

#define xglGetCharModel(character)                       \
  do {                                                   \
    model.size[AXIS_X] = set->face->glyph->bitmap.width; \
    model.size[AXIS_Y] = set->face->glyph->bitmap.rows;  \
  } while (false)

uint32_t ideUpdateTextureAtlas(IDE *ide, Array /*<char_t>*/ *char_array, CharModelSet *set) {
  const Allocator *allocator = ide->allocator;
  TextureAtlas *atlas = Array_real_addr(ide->atlasManager, set->atlas - 1);
  uint32_t width = atlas->width, max_width = atlas->width, max_height = atlas->height;
  CharModel model = {};
  Array *update_array = Array_new(sizeof(char_t), enum_IDE_CHAR, allocator);
  const char_t *character = Array_first_real(char_array);
  const char_t *last = Array_last_real(char_array);
  for (; character <= last; character++) {
    if (AVLTree_get(set->charTree, *character)) { continue; }
    FT_Error err = FT_Load_Char(set->face, *character, FT_LOAD_RENDER);
    if (err) {
      rt_error("Loading font %s of '%c' failed", set->font.path, *character);
      continue;
    }
    Array_append(update_array, character, 1);
    xglGetCharModel(character);
    width += model.size[AXIS_X];
    max_width = max(max_width, model.size[AXIS_X]);
    max_height = max(max_height, model.size[AXIS_Y]);
  }
  if (Array_length(update_array) == 0) {
    releasePrimeArray(update_array);
    return 0;
  }
  uint32_t old_texture = atlas->texture;
  glCreateTextures(GL_TEXTURE_2D, 1, &atlas->texture);
  glTextureStorage2D(atlas->texture, 1, GL_R8, (GLsizei) width, (GLsizei) max_height);
  if (old_texture) {
    glCopyImageSubData(old_texture, GL_TEXTURE_2D, 0, 0, 0, 0, atlas->texture, GL_TEXTURE_2D, 0, 0,
                       0, 0, (GLsizei) atlas->width, (GLsizei) atlas->height, 0);
    glDeleteTextures(1, &old_texture);
  }
  uint32_t x = atlas->width;
  character = Array_first_real(update_array);
  last = Array_last_real(update_array);
  for (; character <= last; character++) {
    FT_Load_Char(set->face, *character, FT_LOAD_RENDER);
    xglGetCharModel(character);
    const uint8_t *data = set->face->glyph->bitmap.buffer;
    glTextureSubImage2D(atlas->texture, 0, (GLint) x, 0, (GLsizei) model.size[AXIS_X],
                        (GLsizei) model.size[AXIS_Y], GL_RED, GL_UNSIGNED_BYTE, data);
    model.texture = x;
    Array_append(set->modelArray, &model, 1);
    AVLTree_set(set->charTree, *character, Array_last_virt(set->modelArray));
    x += model.size[AXIS_X];
  }
  atlas->width = width;
  atlas->height = max_height;

  glTextureParameteri(atlas->texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTextureParameteri(atlas->texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTextureParameteri(atlas->texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(atlas->texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  uint32_t count = Array_length(update_array);
  releasePrimeArray(update_array);
  return count;
}

enum RECT_CORNER {
  RC_LT,
  RC_LB,
  RC_RT,
  RC_RB,
};

inline DrawTask *ideCreatePrint2D(IDE *ide, Array /*<char_t>*/ *char_array,
                                  Array /*<PixelVertex2D>*/ *vertex_array, uint32_t plane_index,
                                  Font *font) {
  if (!ide || !char_array || !vertex_array || !font) { return nullptr; }
  const Allocator *allocator = ide->allocator;
  CharModelSet *set = ideGenCharModelSet(ide, font);
  ideUpdateTextureAtlas(ide, char_array, set);
  constexpr float scale = 0.5f;
  TextureAtlas *atlas = Array_real_addr(ide->atlasManager, set->atlas - 1);
  Array *coord_array = Array_new(sizeof(XGLCoord), enum_XGL_VERTEX, allocator);
  Array *color_array = Array_new(sizeof(XGLColor), enum_XGL_COLOR, allocator);
  Array *tex_coord_array = Array_new(sizeof(XGLColor), enum_XGL_COLOR, allocator);
  Array *index_array = Array_new(sizeof(GLuint), enum_XGL_INDEX, allocator);
  uint32_t count = min(Array_length(vertex_array), Array_length(char_array));
  const Vertex2D * const vertices = Array_real_addr(vertex_array, 0);
  const char_t * const string = Array_real_addr(char_array, 0);
  for (uint32_t i = 0; i < count; i++) {
    CharModel *model = AVLTree_get(set->charTree, string[i]);
    model = Array_vert2real(set->modelArray, model);
    XGLCoord coords[4] = {};
    XGLColor colors[4] = {};
    XGLTexCoord tex_coords[4] = {};
    coords[RC_LT][AXIS_X] =
      (float) vertices[i].coord[AXIS_X] - ((float) model->size[AXIS_X]) * scale;
    coords[RC_LT][AXIS_Y] =
      (float) vertices[i].coord[AXIS_Y] - ((float) model->size[AXIS_Y]) * scale;
    coords[RC_LT][AXIS_Z] = (float) plane_index;
    coords[RC_LT][AXIS_W] = 0.0f;
    coords[RC_RT][AXIS_X] =
      (float) vertices[i].coord[AXIS_X] + ((float) model->size[AXIS_X]) * scale;
    coords[RC_RT][AXIS_Y] =
      (float) vertices[i].coord[AXIS_Y] - ((float) model->size[AXIS_Y]) * scale;
    coords[RC_RT][AXIS_Z] = (float) plane_index;
    coords[RC_RT][AXIS_W] = 0.0f;
    coords[RC_LB][AXIS_X] =
      (float) vertices[i].coord[AXIS_X] - ((float) model->size[AXIS_X]) * scale;
    coords[RC_LB][AXIS_Y] =
      (float) vertices[i].coord[AXIS_Y] + ((float) model->size[AXIS_Y]) * scale;
    coords[RC_LB][AXIS_Z] = (float) plane_index;
    coords[RC_LB][AXIS_W] = 0.0f;
    coords[RC_RB][AXIS_X] =
      (float) vertices[i].coord[AXIS_X] + ((float) model->size[AXIS_X]) * scale;
    coords[RC_RB][AXIS_Y] =
      (float) vertices[i].coord[AXIS_Y] + ((float) model->size[AXIS_Y]) * scale;
    coords[RC_RB][AXIS_Z] = (float) plane_index;
    coords[RC_RB][AXIS_W] = 0.0f;
    for (int j = 0; j < 4; j++) { rgba2XGLColor(vertices[i].color, &colors[j]); }
    tex_coords[RC_LT][AXIS_X] = (float) model->texture / (float) atlas->width;
    tex_coords[RC_LT][AXIS_Y] = (float) 0.0f;
    tex_coords[RC_RT][AXIS_X] =
      (float) (model->texture + model->size[AXIS_X]) / (float) atlas->width;
    tex_coords[RC_RT][AXIS_Y] = (float) 0.0f;
    tex_coords[RC_LB][AXIS_X] = (float) model->texture / (float) atlas->width;
    tex_coords[RC_LB][AXIS_Y] = (float) model->size[AXIS_Y] / (float) atlas->height;
    tex_coords[RC_RB][AXIS_X] =
      (float) (model->texture + model->size[AXIS_X]) / (float) atlas->width;
    tex_coords[RC_RB][AXIS_Y] = (float) model->size[AXIS_Y] / (float) atlas->height;
    Array_append(coord_array, coords, 4);
    Array_append(color_array, colors, 4);
    Array_append(tex_coord_array, tex_coords, 4);
    GLuint indices[6] = {i * 4, i * 4 + 1, i * 4 + 2, i * 4 + 1, i * 4 + 2, i * 4 + 3};
    Array_append(index_array, indices, 6);
  }
  DrawTask * const task = xglCreateTexturedDrawTask(coord_array, color_array, tex_coord_array,
                                                    index_array, ide->allocator);
  task->task_type = TT_TEXT;
  task->texture = atlas->texture;
  task->texture_unit = atlas->unit;

  releasePrimeArray(coord_array);
  releasePrimeArray(color_array);
  releasePrimeArray(tex_coord_array);
  releasePrimeArray(index_array);

  return task;
}

