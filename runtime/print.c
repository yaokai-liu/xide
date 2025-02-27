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
 * Filename: print.c
 * Creator: Yaokai Liu
 * Create Date: 2025-02-22
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include <minmax.h>
#include "print.h"
#include "draw.h"
#include "ide.h"
#include "runtime.h"
#include "utils.h"
#include "font-manage.h"

void xglPrintText(const DrawTask *task, const GLfloat viewport[4]) {
  glUseProgram(task->program);
  glBindVertexArray(task->VAO);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBindTextureUnit(task->texture_unit, task->texture);
  int loc_viewport = glGetUniformLocation(task->program, "viewport");
  int loc_tex = glGetUniformLocation(task->program, "tex");
  glProgramUniform4fv(task->program, loc_viewport, 1, viewport);
  glProgramUniform1i(task->program, loc_tex, (GLint) task->texture_unit);
  glDrawElements(GL_TRIANGLES, task->n_index, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

inline void
xglGenCharCoord2D(const CharModel *model, const Vertex2D *center, const TextureAtlas *atlas, XGLVertex dest[4]) {
  constexpr float scale = 0.5f;
  dest[RC_LT].coord[AXIS_X] = ((float) center->coord[AXIS_X]) - ((float) model->size[AXIS_X]) * scale;
  dest[RC_LT].coord[AXIS_Y] = ((float) center->coord[AXIS_Y]) - ((float) model->size[AXIS_Y]) * scale;
  dest[RC_RT].coord[AXIS_X] = ((float) center->coord[AXIS_X]) + ((float) model->size[AXIS_X]) * scale;
  dest[RC_RT].coord[AXIS_Y] = ((float) center->coord[AXIS_Y]) - ((float) model->size[AXIS_Y]) * scale;
  dest[RC_LB].coord[AXIS_X] = ((float) center->coord[AXIS_X]) - ((float) model->size[AXIS_X]) * scale;
  dest[RC_LB].coord[AXIS_Y] = ((float) center->coord[AXIS_Y]) + ((float) model->size[AXIS_Y]) * scale;
  dest[RC_RB].coord[AXIS_X] = ((float) center->coord[AXIS_X]) + ((float) model->size[AXIS_X]) * scale;
  dest[RC_RB].coord[AXIS_Y] = ((float) center->coord[AXIS_Y]) + ((float) model->size[AXIS_Y]) * scale;
  rgba2XGLColor(center->color, &dest[RC_LT].color);
  rgba2XGLColor(center->color, &dest[RC_RT].color);
  rgba2XGLColor(center->color, &dest[RC_LB].color);
  rgba2XGLColor(center->color, &dest[RC_RB].color);
  dest[RC_LT].tex_coord[AXIS_X] = ( (float) model->offset - 1) / (float) atlas->width;
  dest[RC_LT].tex_coord[AXIS_Y] = 0.0f;
  dest[RC_RT].tex_coord[AXIS_X] = (float) (model->offset + model->size[AXIS_X]) / (float) atlas->width;
  dest[RC_RT].tex_coord[AXIS_Y] = 0.0f;
  dest[RC_LB].tex_coord[AXIS_X] = (float) (model->offset - 1) / (float) atlas->width;
  dest[RC_LB].tex_coord[AXIS_Y] = (float) model->size[AXIS_Y] / (float) atlas->height;
  dest[RC_RB].tex_coord[AXIS_X] = (float) (model->offset + model->size[AXIS_X]) / (float) atlas->width;
  dest[RC_RB].tex_coord[AXIS_Y] = (float) model->size[AXIS_Y] / (float) atlas->height;
}
Array/*<Vertex2D>*/ *ideGenCharCoordArray(const CharModelSet *set, const Array /*<char_t>*/ *char_array,
                                          const Vertex2D * anchor, const int32_t c_space, const uint32_t mode,
                                          const Allocator *allocator) {
  Array *vertex_array = Array_new(sizeof(Vertex2D), enum_XGL_COORD, allocator);
  const uint32_t count = Array_length(char_array);
  const char_t * const string = Array_real_addr(char_array, 0);
  float offset_x = 0, offset_y = 0;
  uint32_t origin = 0, height = 0, width = 0;
  for (uint32_t i = 0; i < count; i++) {
    const CharModel *model = AVLTree_get(set->charTree, string[i]);
    model = Array_vert2real(set->modelArray, model);
    offset_x  = ((float) model->size[AXIS_X]) / 2 + ((float) model->bearing[AXIS_X]);
    offset_y  = ((float) model->bearing[AXIS_Y]) - ((float) model->size[AXIS_Y]) / 2;
    Vertex2D vertex = {.coord = { [AXIS_X] = offset_x +(float)  origin, [AXIS_Y] = offset_y },
                       .color = anchor->color };
    Array_append(vertex_array, &vertex, 1);
    height = max(height, model->size[AXIS_Y]);
    origin += (model->advance[AXIS_X] >> 6) + c_space;
  }
  width = origin;

  Vertex2D *vertices = Array_real_addr(vertex_array, 0);
  for (uint32_t i = 0; i < count; i++) {
    vertices[i].coord[AXIS_Y] = ((float) height) - vertices[i].coord[AXIS_Y];
    vertices[i].coord[AXIS_X] += anchor->coord[AXIS_X];
    vertices[i].coord[AXIS_Y] += anchor->coord[AXIS_Y];
  }
  switch (mode & TS_H_MASK) {
    case TS_H_CENTER: {
      for (uint32_t i = 0; i < count; i++) { vertices[i].coord[AXIS_X] -= (float) width / 2; }
      break;
    }
    case TS_LEFT: {
      for (uint32_t i = 0; i < count; i++) { vertices[i].coord[AXIS_X] -= (float) width; }
      break;
    }
  }
  switch (mode & TS_V_MASK) {
    case TS_V_CENTER: {
      for (uint32_t i = 0; i < count; i++) { vertices[i].coord[AXIS_Y] -= (float) height / 2; }
      break;
    }
    case TS_ABOVE: {
      for (uint32_t i = 0; i < count; i++) { vertices[i].coord[AXIS_Y] -= (float) height; }
      break;
    }
  }
  return vertex_array;
}
