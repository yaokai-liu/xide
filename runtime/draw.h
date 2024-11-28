/**
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

DrawTask *xglCreateDrawTask(Array *vertex_array, Array *color_array, Array *index_array,
                            const Allocator *allocator);
void xglDestroyDrawTask(DrawTask *task);

DrawTask *xglCreatePolygon2D(Array *array, int plane_index, bool solid, const Allocator *allocator);
DrawTask *xglCreateCurveArea2D(Array *array, int plane_index, bool cycle, bool solid,
                               const Allocator *allocator);

DrawTask *xglCreatePixelLines(Line *lines, int count, int plane_index, const Allocator *allocator);
DrawTask *xglCreatePixelPolygon(PixelVertex *vertices, int count, int plane_index, bool solid,
                                const Allocator *allocator);

DrawTask *xglCreatePolyline2D(Array *vertex_array, int plane_index, bool cycle,
                              const Allocator *allocator);
DrawTask *xglCreatePixelPolyline(PixelVertex *vertices, int count, int plane_index, bool cycle,
                                 const Allocator *allocator);

void xglBindShaderProgram(DrawTask *task, GLuint program);

void xglDrawLines(const DrawTask *task, const GLfloat viewportSize[2]);
void xglDrawArea(const DrawTask *task, const GLfloat viewportSize[2]);
void xglDrawPolyline(const DrawTask *task, const GLfloat viewportSize[2]);
void xglDraw(const DrawTask *task, const IdeWindow *window);

#endif  // XIDE_DRAW_H
