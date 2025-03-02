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
 * Module Name: runtime
 * Filename: draw.h
 * Creator: Yaokai Liu
 * Create Date: 2024-7-7
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_DRAW_H
#define XIDE_DRAW_H

#include "array.h"
#include "texture-manage.h"
#include "widgets.h"
#include "xgl-object.h"
#include "ide.h"

enum TASK_TYPE_ENUM {
  TT_LINES = 1,
  TT_SOLID_AREA,
  TT_TRIANGULATED_AREA,
  TT_POLYLINE,
  TT_TEXT,
};

typedef struct DrawTask {
  uint32_t task_type;
  iXGLVao VAO;
  iXGLshProg program;
  iXGLIbo IBO;
  GLsizei n_index;
  Array *VBOs;  // Array<iXGLVbo>
  Array *uniforms;  // Array<iXGLVUniform>
  uint32_t atlas_index;
  uint32_t texture_unit;
} DrawTask;

DrawTask *ideCreateNonTextureDrawTask(const Array * vertex_array, const Array * color_array,
                                      const Array * index_array, const Allocator * allocator);
DrawTask * ideCreateTexturedDrawTask(const Array *vertex_array, const Array *index_array, const TextureAtlas *atlas,
                                     const Allocator *allocator);
void xglDestroyDrawTask(DrawTask *task, const Allocator *allocator);

DrawTask *ideCreatePolygon2D(const Array *vertex_array, float plane_index, bool solid, const Allocator *allocator);
DrawTask *ideCreateCurveArea2D(const Array *vertex_array, float plane_index, bool cycle, bool solid,
                               const Allocator *allocator);
DrawTask *ideCreatePolyline2D(const Array *vertex_array, float plane_index, bool cycle, const Allocator *allocator);

DrawTask *ideCreatePixelLines(const Array *line_array, int plane_index, const Allocator *allocator);
DrawTask *ideCreatePixelPolygon2D(const Array *vertex_array, int plane_index, bool solid, const Allocator *allocator);
DrawTask *ideCreatePixelPolyline2D(const Array *vertex_array, int plane_index, bool cycle, const Allocator *allocator);

DrawTask *ideCreateDrawTextTask(IDE *ide, const Array *char_array, const Array *vert_array, const CharModelSet *set,
                                uint32_t plane_index, const Font *font);
DrawTask *ideCreateTextStr2D(IDE *ide, const Array *char_array, const Vertex2D *anchor, float c_space, uint32_t mode,
                             uint32_t plane_index, const Font *font, XGLVector2D feedback_vec);
DrawTask *ideCreateTextStr2DByStr(IDE *ide, const char_t *string, const Vertex2D *anchor, float c_space, uint32_t mode,
                                  uint32_t plane_index, const Font *font, XGLVector2D feedback_vec);

void xglBindShaderProgram(DrawTask *task, GLuint program);

void ideDrawLines(const DrawTask *task, const GLfloat viewport[2]);
void ideDrawArea(const DrawTask *task, const GLfloat viewport[2]);
void ideDrawPolyline(const DrawTask *task, const GLfloat viewport[2]);
void ideDrawText(IDE *ide, const DrawTask *task, const GLfloat viewport[2]);
void ideDraw(const DrawTask * task, IDE * ide);

#endif  // XIDE_DRAW_H
