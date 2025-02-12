/**
 * Project Name: xide
 * Module Name: runtime
 * Filename: draw.c
 * Creator: Yaokai Liu
 * Create Date: 2024-7-7
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "draw.h"
#include "GLFW/glfw3.h"
#include "cg2d.h"
#include "glad/glad.h"
#include "utils.h"
#include "widgets.h"
#include "xgl-object.h"
#include <math.h>

inline DrawTask *xglCreateDrawTask(const Array * const vertex_array,
                                   const Array * const color_array, const Array * const index_array,
                                   const Allocator * const allocator) {
  iXGLVao VAO = {};
  glCreateVertexArrays(1, &VAO);
  glEnableVertexArrayAttrib(VAO, LOC_VERTEX);
  glEnableVertexArrayAttrib(VAO, LOC_COLOR);
  glEnableVertexArrayAttrib(VAO, LOC_WINDOW_SIZE);

  iXGLVbo VBOs[3] = {};
  glCreateBuffers(3, VBOs);
  glNamedBufferStorage(VBOs[0], Array_length(vertex_array) * (GLsizeiptr) sizeof(XGLCoord),
                       Array_get(vertex_array, 0), 0);
  glNamedBufferStorage(VBOs[1], Array_length(color_array) * (GLsizeiptr) sizeof(XGLColor),
                       Array_get(color_array, 0), 0);
  glNamedBufferStorage(VBOs[2], Array_length(index_array) * (GLsizeiptr) sizeof(GLint),
                       Array_get(index_array, 0), 0);

  glVertexArrayAttribBinding(VAO, LOC_VERTEX, 0);
  glVertexArrayAttribFormat(VAO, LOC_VERTEX, sizeof(XGLCoord) / sizeof(GLfloat), GL_FLOAT, GL_FALSE,
                            0);
  glVertexArrayVertexBuffer(VAO, 0, VBOs[0], 0, sizeof(XGLCoord));

  glVertexArrayAttribBinding(VAO, LOC_COLOR, 1);
  glVertexArrayAttribFormat(VAO, LOC_COLOR, sizeof(XGLColor) / sizeof(GLfloat), GL_FLOAT, GL_FALSE,
                            0);
  glVertexArrayVertexBuffer(VAO, 1, VBOs[1], 0, sizeof(XGLColor));

  glVertexArrayElementBuffer(VAO, VBOs[2]);

  DrawTask *task = allocator->calloc(1, sizeof(DrawTask));
  task->VAO = VAO;
  task->VBOs = Array_new(sizeof(iXGLVbo), allocator);
  task->IBO = VBOs[2];
  task->n_index = (GLsizei) Array_length(index_array);
  task->uniforms = Array_new(sizeof(iXGLVUniform), allocator);

  Array_append(task->VBOs, VBOs, 2);
  iXGLVUniform uniform = {uniform_type(US_2SCA, UD_INT), LOC_WINDOW_SIZE};
  Array_append(task->uniforms, &uniform, 1);

  return task;
}

inline void xglDestroyDrawTask(DrawTask * const task) {
  const iXGLVbo *buffer = (iXGLVbo *) Array_get(task->VBOs, 0);
  glDeleteBuffers((GLint) Array_length(task->VBOs), buffer);
  glDeleteVertexArrays(1, &task->VAO);

  releaseArray(task->VBOs);
  releaseArray(task->uniforms);
}

void xglBindShaderProgram(DrawTask *task, GLuint program) {
  task->program = program;
}

DrawTask *xglCreatePixelLines(const Array * const line_array, const int plane_index,
                              const Allocator * const allocator) {
  const int count = (int) Array_length(line_array);
  const Line * const lines = Array_get(line_array, 0);
  Array *vertex_array = Array_new(sizeof(XGLCoord), allocator);
  Array *color_array = Array_new(sizeof(XGLColor), allocator);
  Array *index_array = Array_new(sizeof(GLint), allocator);
  for (int i = 0; i < count; i++) {
    XGLCoord vertices[2] = {};
    XGLColor colors[2] = {};
    GLint indices[2] = {2 * i, 2 * i + 1};
    rgba2XGLColor(lines[i][0].color, &colors[0]);
    rgba2XGLColor(lines[i][1].color, &colors[1]);
    vertices[VERTEX_BEGIN][AXIS_X] = (float) lines[i][0].coord[AXIS_X];
    vertices[VERTEX_BEGIN][AXIS_Y] = (float) lines[i][0].coord[AXIS_Y];
    vertices[VERTEX_BEGIN][AXIS_Z] = atanf((float) plane_index) * 100.0f;
    vertices[VERTEX_BEGIN][AXIS_W] = 0.0f;
    vertices[VERTEX_END][AXIS_X] = (float) lines[i][1].coord[AXIS_X];
    vertices[VERTEX_END][AXIS_Y] = (float) lines[i][1].coord[AXIS_Y];
    vertices[VERTEX_END][AXIS_Z] = atanf((float) plane_index) * 100.0f;
    vertices[VERTEX_END][AXIS_W] = 0.0f;
    Array_append(vertex_array, vertices, 2);
    Array_append(color_array, colors, 2);
    Array_append(index_array, indices, 2);
  }

  DrawTask * const task = xglCreateDrawTask(vertex_array, color_array, index_array, allocator);
  task->task_type = TT_LINES;

  releaseArray(vertex_array);
  releaseArray(color_array);
  releaseArray(index_array);

  return task;
}

DrawTask *xglCreatePolygon2D(const Array * const vertex_array, const int plane_index,
                             const bool solid, const Allocator * const allocator) {
  const int count = (int) Array_length(vertex_array);
  const Vertex * const vertices = Array_get(vertex_array, 0);
  Array *coord_array = Array_new(sizeof(XGLCoord), allocator);
  Array *color_array = Array_new(sizeof(XGLColor), allocator);
  for (int i = 0; i < count; i++) {
    XGLCoord vertex = {};
    XGLColor color = {};
    rgba2XGLColor(vertices[i].color, &color);
    vertex[AXIS_X] = vertices[i].coord[AXIS_X];
    vertex[AXIS_Y] = vertices[i].coord[AXIS_Y];
    vertex[AXIS_Z] = atanf((float) plane_index) * 100.0f;
    vertex[AXIS_W] = 0.0f;
    Array_append(coord_array, vertex, 1);
    Array_append(color_array, color, 1);
  }
  Array *index_array = xglEarClippingTriangulate2D(coord_array, allocator);

  DrawTask * const task = xglCreateDrawTask(coord_array, color_array, index_array, allocator);
  task->task_type = solid ? TT_SOLID_AREA : TT_TRIANGULATED_AREA;

  releaseArray(coord_array);
  releaseArray(color_array);
  releaseArray(index_array);

  return task;
}

DrawTask *xglCreateCurveArea2D(const Array * const vertex_array, const int plane_index,
                               const bool cycle, const bool solid,
                               const Allocator * const allocator) {
  const int count = (int) Array_length(vertex_array);
  const Vertex * const vertices = Array_get(vertex_array, 0);
  Array *coord_array = Array_new(sizeof(XGLCoord), allocator);
  Array *color_array = Array_new(sizeof(XGLColor), allocator);
  for (int i = 0; i < count; i++) {
    XGLCoord vertex = {};
    XGLColor color = {};
    rgba2XGLColor(vertices[i].color, &color);
    vertex[AXIS_X] = vertices[i].coord[AXIS_X];
    vertex[AXIS_Y] = vertices[i].coord[AXIS_Y];
    vertex[AXIS_Z] = atanf((float) plane_index) * 100.0f;
    vertex[AXIS_W] = 0.0f;
    Array_append(coord_array, vertex, 1);
    Array_append(color_array, color, 1);
  }
  Array *index_array = xglRadialTriangulation2D(coord_array, cycle, allocator);

  DrawTask * const task = xglCreateDrawTask(coord_array, color_array, index_array, allocator);
  task->task_type = solid ? TT_SOLID_AREA : TT_TRIANGULATED_AREA;

  releaseArray(coord_array);
  releaseArray(color_array);
  releaseArray(index_array);

  return task;
}

DrawTask *xglCreatePixelPolygon(const Array * const vertex_array, int plane_index, bool solid,
                                const Allocator *allocator) {
  const int count = (int) Array_length(vertex_array);
  const Vertex * const vertices = Array_get(vertex_array, 0);
  Array *coord_array = Array_new(sizeof(XGLCoord), allocator);
  Array *color_array = Array_new(sizeof(XGLColor), allocator);
  for (int i = 0; i < count; i++) {
    XGLCoord vertex = {};
    XGLColor color = {};
    rgba2XGLColor(vertices[i].color, &color);
    vertex[AXIS_X] = (float) vertices[i].coord[AXIS_X];
    vertex[AXIS_Y] = (float) vertices[i].coord[AXIS_Y];
    vertex[AXIS_Z] = atanf((float) plane_index) * 100.0f;
    vertex[AXIS_W] = 0.0f;
    Array_append(coord_array, vertex, 1);
    Array_append(color_array, color, 1);
  }
  Array *index_array = xglEarClippingTriangulate2D(coord_array, allocator);

  DrawTask * const task = xglCreateDrawTask(coord_array, color_array, index_array, allocator);
  task->task_type = solid ? TT_SOLID_AREA : TT_TRIANGULATED_AREA;

  releaseArray(coord_array);
  releaseArray(color_array);
  releaseArray(index_array);

  return task;
}

DrawTask *xglCreatePolyline2D(const Array * const vertex_array, const int plane_index,
                              const bool cycle, const Allocator * const allocator) {
  const int count = (int) Array_length(vertex_array);
  const Vertex * const vertices = Array_get(vertex_array, 0);
  Array *coord_array = Array_new(sizeof(XGLCoord), allocator);
  Array *color_array = Array_new(sizeof(XGLColor), allocator);
  Array *index_array = Array_new(sizeof(GLint), allocator);
  for (int i = 0; i < count; i++) {
    XGLCoord vertex = {};
    XGLColor color = {};
    rgba2XGLColor(vertices[i].color, &color);
    vertex[AXIS_X] = vertices[i].coord[AXIS_X];
    vertex[AXIS_Y] = vertices[i].coord[AXIS_Y];
    vertex[AXIS_Z] = atanf((float) plane_index) * 100.0f;
    vertex[AXIS_W] = 0.0f;
    Array_append(coord_array, vertex, 1);
    Array_append(color_array, color, 1);
  }
  for (int i = 0; i < count - 1; i++) {
    int indices[2] = {i, i + 1};
    Array_append(index_array, indices, 2);
  }
  if (cycle) {
    int indices[2] = {count - 1, 0};
    Array_append(index_array, indices, 2);
  }

  DrawTask * const task = xglCreateDrawTask(coord_array, color_array, index_array, allocator);
  task->task_type = TT_POLYLINE;

  releaseArray(coord_array);
  releaseArray(color_array);
  releaseArray(index_array);

  return task;
}

DrawTask *xglCreatePixelPolyline(const Array * const vertex_array, int plane_index, bool cycle,
                                 const Allocator *allocator) {
  const int count = (int) Array_length(vertex_array);
  const Vertex * const vertices = Array_get(vertex_array, 0);
  Array *coord_array = Array_new(sizeof(XGLCoord), allocator);
  Array *color_array = Array_new(sizeof(XGLColor), allocator);
  Array *index_array = Array_new(sizeof(GLint), allocator);
  for (int i = 0; i < count; i++) {
    XGLCoord vertex = {};
    XGLColor color = {};
    rgba2XGLColor(vertices[i].color, &color);
    vertex[AXIS_X] = (float) vertices[i].coord[AXIS_X];
    vertex[AXIS_Y] = (float) vertices[i].coord[AXIS_Y];
    vertex[AXIS_Z] = atanf((float) plane_index) * 100.0f;
    vertex[AXIS_W] = 0.0f;
    Array_append(coord_array, vertex, 1);
    Array_append(color_array, color, 1);
  }
  for (int i = 0; i < count - 1; i++) {
    int indices[2] = {i, i + 1};
    Array_append(index_array, indices, 2);
  }
  if (cycle) {
    int indices[2] = {count - 1, 0};
    Array_append(index_array, indices, 2);
  }

  DrawTask * const task = xglCreateDrawTask(coord_array, color_array, index_array, allocator);
  task->task_type = TT_POLYLINE;

  releaseArray(coord_array);
  releaseArray(color_array);
  releaseArray(index_array);

  return task;
}

inline void xglDrawLines(const DrawTask * const task, const GLfloat viewportSize[2]) {
  glUseProgram(task->program);
  glBindVertexArray(task->VAO);
  for (uint32_t i = 0; i < Array_length(task->uniforms); i++) {
    iXGLVUniform *uniform = (iXGLVUniform *) Array_get(task->uniforms, i);
    glProgramUniform2fv(task->program, uniform->u_locate, 1, viewportSize);
  }
  glDrawElements(GL_LINES, task->n_index, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

inline void xglDrawArea(const DrawTask * const task, const GLfloat viewportSize[2]) {
  glUseProgram(task->program);
  glBindVertexArray(task->VAO);
  if (task->task_type == TT_SOLID_AREA) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  for (uint32_t i = 0; i < Array_length(task->uniforms); i++) {
    iXGLVUniform *uniform = (iXGLVUniform *) Array_get(task->uniforms, i);
    glProgramUniform2fv(task->program, uniform->u_locate, 1, viewportSize);
  }
  glDrawElements(GL_TRIANGLES, task->n_index, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

inline void xglDrawPolyline(const DrawTask * const task, const GLfloat viewportSize[2]) {
  glUseProgram(task->program);
  glBindVertexArray(task->VAO);
  for (uint32_t i = 0; i < Array_length(task->uniforms); i++) {
    iXGLVUniform *uniform = (iXGLVUniform *) Array_get(task->uniforms, i);
    glProgramUniform2fv(task->program, uniform->u_locate, 1, viewportSize);
  }
  glDrawElements(GL_LINE_STRIP, task->n_index, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

inline void xglDraw(const DrawTask * const task, const IdeWindow * const window) {
  const float * const viewSize = &window->viewport[2];
  switch (task->task_type) {
    case TT_LINES: {
      return xglDrawLines(task, viewSize);
    }
    case TT_POLYLINE: {
      return xglDrawPolyline(task, viewSize);
    }
    case TT_SOLID_AREA:
    case TT_TRIANGULATED_AREA: {
      return xglDrawArea(task, viewSize);
    }
  }
}
