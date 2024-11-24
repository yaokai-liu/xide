/**
 * Project Name: xide
 * Module Name: runtime
 * Filename: drawUI.h
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
  TT_POLYLINE = 3,
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
                            MainWindow *window);
void xglDestroyDrawTask(DrawTask *task);

DrawTask *xglCreateLines(Line *lines, int count, int plane_index, MainWindow *window);
DrawTask *xglCreateSolidPolygon(Vertex *vertices, int count, int plane_index, MainWindow *window);
DrawTask *xglCreatePolyline(Vertex *vertices, int count, bool cycle, int plane_index,
                            MainWindow *window);

void xglDrawLines(DrawTask *task, const GLfloat windowSize[2]);
void xglDrawSolidArea(DrawTask *task, const GLfloat windowSize[2]);
void xglDrawPolyline(DrawTask *task, const GLfloat windowSize[2]);

#endif  // XIDE_DRAW_H
