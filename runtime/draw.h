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
#include "widgets.h"
#include "xgl-object.h"

enum TASK_TYPE_ENUM {
  TT_LINES = 1,
  TT_SOLID_AREA = 2,
  TT_TRIANGULATED_AREA = 3,
  TT_POLYLINE = 4,
};

typedef struct DrawTask {
  uint32_t task_type;
  iXGLVao VAO;
  iXGLshProg program;
  iXGLIbo IBO;
  GLsizei n_index;
  Array *VBOs;  // Array<iXGLVbo>
  Array *uniforms;  // Array<iXGLVUniform>
} DrawTask;

DrawTask *xglCreateDrawTask(const Array *vertex_array, const Array *color_array,
                            const Array *index_array, const Allocator *allocator);
void xglDestroyDrawTask(DrawTask *task, const Allocator *allocator);

DrawTask *xglCreatePolygon2D(const Array *vertex_array, float plane_index, bool solid,
                             const Allocator *allocator);
DrawTask *xglCreateCurveArea2D(const Array *vertex_array, float plane_index, bool cycle, bool solid,
                               const Allocator *allocator);
DrawTask *xglCreatePolyline2D(const Array *vertex_array, float plane_index, bool cycle,
                              const Allocator *allocator);

DrawTask *xglCreatePixelLines(const Array *line_array, int plane_index, const Allocator *allocator);
DrawTask *xglCreatePixelPolygon(const Array *vertex_array, int plane_index, bool solid,
                                const Allocator *allocator);
DrawTask *xglCreatePixelPolyline(const Array *vertex_array, int plane_index, bool cycle,
                                 const Allocator *allocator);

void xglBindShaderProgram(DrawTask *task, GLuint program);

void xglDrawLines(const DrawTask *task, const GLfloat viewport[2]);
void xglDrawArea(const DrawTask *task, const GLfloat viewport[2]);
void xglDrawPolyline(const DrawTask *task, const GLfloat viewport[2]);
void xglDraw(const DrawTask *task, const IdeWindow *window);

#endif  // XIDE_DRAW_H
