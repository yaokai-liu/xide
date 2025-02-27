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

uint32_t ideUpdateTextureAtlas(IDE *ide, Array /*<char_t>*/ *char_array, CharModelSet *set) {
  const Allocator *allocator = ide->allocator;
  TextureAtlas *atlas = Array_real_addr(ide->atlasManager, set->atlas - 1);
  const uint32_t old_width = atlas->width, old_height = atlas->height;
  uint32_t current_width = atlas->width + 1, current_height = atlas->height;
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
    const uint32_t sub_width = set->face->glyph->bitmap.width;
    const uint32_t sub_height = set->face->glyph->bitmap.rows;
    const uint32_t bearing_x  = set->face->glyph->bitmap_left;
    const uint32_t bearing_y  = set->face->glyph->bitmap_top;
    const uint32_t advance_x  = set->face->glyph->advance.x;
    const uint32_t advance_y  = set->face->glyph->advance.y;
    const CharModel model = {
        .code=*character, .offset=current_width,
        .size={ [AXIS_X]=sub_width, [AXIS_Y]=sub_height },
        .bearing={ [AXIS_X]=bearing_x, [AXIS_Y]=bearing_y },
        .advance={ [AXIS_X]=advance_x, [AXIS_Y]=advance_y }
    };
    Array_append(set->modelArray, &model, 1);
    Array_append(update_array, character, 1);
    REFER(CharModel) v_model = Array_last_virt(set->modelArray);
    AVLTree_set(set->charTree, *character, v_model);
    current_height = max(current_height, sub_height + 1);
    current_width += sub_width + 2;
  }
  atlas->width = current_width;
  atlas->height = current_height;
  if (Array_length(update_array) == 0) {
    releasePrimeArray(update_array);
    return 0;
  }
  const uint32_t old_texture = atlas->texture;
  glCreateTextures(GL_TEXTURE_2D, 1, &atlas->texture);
  glTextureStorage2D(atlas->texture, 1, GL_R8,
                     (GLsizei) current_width, (GLsizei) current_height);
  if (old_texture) {
    glCopyImageSubData(old_texture, GL_TEXTURE_2D,
                       0, 0, 0, 0,
                       atlas->texture, GL_TEXTURE_2D,
                       0, 0, 0, 0,
                       (GLsizei) old_width, (GLsizei) old_height, 0);
    glDeleteTextures(1, &old_texture);
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  character = Array_first_real(update_array);
  last = Array_last_real(update_array);
  for (; character <= last; character++) {
    const CharModel *model = AVLTree_get(set->charTree, *character);
    model = Array_vert2real(set->modelArray, model);
    FT_Load_Char(set->face, *character, FT_LOAD_RENDER);
    const uint32_t offset = model->offset;
    const uint8_t *data = set->face->glyph->bitmap.buffer;
    glTextureSubImage2D(atlas->texture, 0, (GLint) offset, 0,
                        (GLsizei) model->size[AXIS_X], (GLsizei) model->size[AXIS_Y],
                        GL_RED, GL_UNSIGNED_BYTE, data);
  }
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

  glTextureParameteri(atlas->texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTextureParameteri(atlas->texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTextureParameteri(atlas->texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(atlas->texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//  const float borderColor[] = {1.0f, 1.0f, 1.0f, 0.0f};
//  glTextureParameterfv(atlas->texture, GL_TEXTURE_BORDER_COLOR, borderColor);

  uint32_t count = Array_length(update_array);
  releasePrimeArray(update_array);
  return count;
}

#define lenof(array) (sizeof(array) / sizeof(typeof((array)[0])))
inline DrawTask *ideCreatePrint2D(IDE *ide, Array /*<char_t>*/ *char_array, Array /*<Vertex2D>*/ *vert_array,
                                  uint32_t plane_index, Font *font) {
  if (!ide || !char_array || !vert_array || !font) { return nullptr; }
  const Allocator *allocator = ide->allocator;
  CharModelSet *set = ideGenCharModelSet(ide, font);
  ideUpdateTextureAtlas(ide, char_array, set);
  TextureAtlas *atlas = Array_real_addr(ide->atlasManager, set->atlas - 1);
  Array *vertex_array = Array_new(sizeof(XGLVertex), enum_XGL_VERTEX, allocator);
  Array *index_array = Array_new(sizeof(GLuint), enum_XGL_INDEX, allocator);
  const uint32_t count = min(Array_length(vert_array), Array_length(char_array));
  const Vertex2D * const pixel_vertices = Array_real_addr(vert_array, 0);
  const char_t * const string = Array_real_addr(char_array, 0);
  for (uint32_t i = 0; i < count; i++) {
    const CharModel *model = AVLTree_get(set->charTree, string[i]);
    model = Array_vert2real(set->modelArray, model);
    XGLVertex vertices[4] = {};
    xglGenCharCoord2D(model, &pixel_vertices[i], atlas, vertices);
    GLuint indices[6] = {
        i * 4 + RC_LT, i * 4 + RC_RT, i * 4 + RC_LB,
        i * 4 + RC_RT, i * 4 + RC_LB, i * 4 + RC_RB
    };
    Array_append(vertex_array, vertices, lenof(vertices));
    Array_append(index_array, indices, lenof(indices));
  }

  DrawTask * const task = xglCreateTexturedDrawTask(vertex_array, index_array, allocator);
  task->task_type = TT_TEXT;
  task->texture = atlas->texture;
  task->texture_unit = atlas->unit;

  releasePrimeArray(vertex_array);
  releasePrimeArray(index_array);

  return task;
}

inline DrawTask *
ideCreateText2D(IDE *ide, Array *char_array, Vertex2D *anchor, const int32_t c_space, const uint32_t mode,
                uint32_t plane_index, Font *font) {
  if (!ide || !char_array || !anchor || !font) { return nullptr; }
  const Allocator *allocator = ide->allocator;
  CharModelSet *set = ideGenCharModelSet(ide, font);
  ideUpdateTextureAtlas(ide, char_array, set);
  Array *vertex_array = ideGenCharCoordArray(set, char_array, anchor, c_space, mode, allocator);
  DrawTask * const task = ideCreatePrint2D(ide, char_array, vertex_array, plane_index, font);
  releasePrimeArray(vertex_array);
  return task;
}
