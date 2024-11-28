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

inline DrawTask *xglCreateDrawTask(Array *vertex_array, Array *color_array, Array *index_array,
                                   const Allocator *allocator) {
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

  Array_reset(task->VBOs, nullptr);
  Array_reset(task->uniforms, nullptr);
  Array_destroy(task->VBOs);
  Array_destroy(task->uniforms);
}

void xglBindShaderProgram(DrawTask *task, GLuint program) {
  task->program = program;
}

DrawTask *xglCreatePixelLines(Line *lines, int count, int plane_index, const Allocator *allocator) {
  Array *vertex_array = Array_new(sizeof(XGLCoord), allocator);
  Array *color_array = Array_new(sizeof(XGLColor), allocator);
  Array *index_array = Array_new(sizeof(GLint), allocator);
  for (int i = 0; i < count; i++) {
    XGLCoord vertices[2] = {};
    XGLColor colors[2] = {};
    GLint indices[2] = {2 * i, 2 * i + 1};
    rgba2XGLColor(lines[i].colors[0], &colors[0]);
    rgba2XGLColor(lines[i].colors[1], &colors[1]);
    vertices[VERTEX_BEGIN][AXIS_X] = (float) lines[i].start[AXIS_X];
    vertices[VERTEX_BEGIN][AXIS_Y] = (float) lines[i].start[AXIS_Y];
    vertices[VERTEX_BEGIN][AXIS_Z] = atanf((float) plane_index) * 100.0f;
    vertices[VERTEX_BEGIN][AXIS_W] = 0.0f;
    vertices[VERTEX_END][AXIS_X] = (float) lines[i].end[AXIS_X];
    vertices[VERTEX_END][AXIS_Y] = (float) lines[i].end[AXIS_Y];
    vertices[VERTEX_END][AXIS_Z] = atanf((float) plane_index) * 100.0f;
    vertices[VERTEX_END][AXIS_W] = 0.0f;
    Array_append(vertex_array, vertices, 2);
    Array_append(color_array, colors, 2);
    Array_append(index_array, indices, 2);
  }

  DrawTask * const task = xglCreateDrawTask(vertex_array, color_array, index_array, allocator);
  task->task_type = TT_LINES;

  Array_reset(vertex_array, nullptr);
  Array_reset(color_array, nullptr);
  Array_reset(index_array, nullptr);
  Array_destroy(vertex_array);
  Array_destroy(color_array);
  Array_destroy(index_array);

  return task;
}

DrawTask *xglCreatePolygon2D(Array *array, int plane_index, bool solid,
                             const Allocator *allocator) {
  const int count = (int) Array_length(array);
  const Vertex * const vertices = Array_get(array, 0);
  Array *vert_array = Array_new(sizeof(XGLCoord), allocator);
  Array *color_array = Array_new(sizeof(XGLColor), allocator);
  for (int i = 0; i < count; i++) {
    XGLCoord vertex = {};
    XGLColor color = {};
    rgba2XGLColor(vertices[i].color, &color);
    vertex[AXIS_X] = vertices[i].coord[AXIS_X];
    vertex[AXIS_Y] = vertices[i].coord[AXIS_Y];
    vertex[AXIS_Z] = atanf((float) plane_index) * 100.0f;
    vertex[AXIS_W] = 0.0f;
    Array_append(vert_array, vertex, 1);
    Array_append(color_array, color, 1);
  }
  Array *index_array = xglEarClippingTriangulate2D(vert_array, allocator);

  DrawTask * const task = xglCreateDrawTask(vert_array, color_array, index_array, allocator);
  task->task_type = solid ? TT_SOLID_AREA : TT_TRIANGULATED_AREA;

  Array_reset(vert_array, nullptr);
  Array_reset(color_array, nullptr);
  Array_reset(index_array, nullptr);
  Array_destroy(vert_array);
  Array_destroy(color_array);
  Array_destroy(index_array);

  return task;
}

DrawTask *xglCreateCurveArea2D(Array *array, int plane_index, bool cycle, bool solid,
                               const Allocator *allocator) {
  const int count = (int) Array_length(array);
  const Vertex * const vertices = Array_get(array, 0);
  Array *vert_array = Array_new(sizeof(XGLCoord), allocator);
  Array *color_array = Array_new(sizeof(XGLColor), allocator);
  for (int i = 0; i < count; i++) {
    XGLCoord vertex = {};
    XGLColor color = {};
    rgba2XGLColor(vertices[i].color, &color);
    vertex[AXIS_X] = vertices[i].coord[AXIS_X];
    vertex[AXIS_Y] = vertices[i].coord[AXIS_Y];
    vertex[AXIS_Z] = atanf((float) plane_index) * 100.0f;
    vertex[AXIS_W] = 0.0f;
    Array_append(vert_array, vertex, 1);
    Array_append(color_array, color, 1);
  }
  Array *index_array = xglRadialTriangulation2D(vert_array, cycle, allocator);

  DrawTask * const task = xglCreateDrawTask(vert_array, color_array, index_array, allocator);
  task->task_type = solid ? TT_SOLID_AREA : TT_TRIANGULATED_AREA;

  Array_reset(vert_array, nullptr);
  Array_reset(color_array, nullptr);
  Array_reset(index_array, nullptr);
  Array_destroy(vert_array);
  Array_destroy(color_array);
  Array_destroy(index_array);

  return task;
}

DrawTask *xglCreatePixelPolygon(PixelVertex *vertices, int count, int plane_index, bool solid,
                                const Allocator *allocator) {
  Array *vert_array = Array_new(sizeof(XGLCoord), allocator);
  Array *color_array = Array_new(sizeof(XGLColor), allocator);
  for (int i = 0; i < count; i++) {
    XGLCoord vertex = {};
    XGLColor color = {};
    rgba2XGLColor(vertices[i].color, &color);
    vertex[AXIS_X] = (float) vertices[i].coord[AXIS_X];
    vertex[AXIS_Y] = (float) vertices[i].coord[AXIS_Y];
    vertex[AXIS_Z] = atanf((float) plane_index) * 100.0f;
    vertex[AXIS_W] = 0.0f;
    Array_append(vert_array, vertex, 1);
    Array_append(color_array, color, 1);
  }
  Array *index_array = xglEarClippingTriangulate2D(vert_array, allocator);

  DrawTask * const task = xglCreateDrawTask(vert_array, color_array, index_array, allocator);
  task->task_type = solid ? TT_SOLID_AREA : TT_TRIANGULATED_AREA;

  Array_reset(vert_array, nullptr);
  Array_reset(color_array, nullptr);
  Array_reset(index_array, nullptr);
  Array_destroy(vert_array);
  Array_destroy(color_array);
  Array_destroy(index_array);

  return task;
}

DrawTask *xglCreatePolyline2D(Array *vertex_array, int plane_index, bool cycle,
                              const Allocator *allocator) {
  const int count = (int) Array_length(vertex_array);
  const Vertex * const vertices = Array_get(vertex_array, 0);
  Array *vert_array = Array_new(sizeof(XGLCoord), allocator);
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
    Array_append(vert_array, vertex, 1);
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

  DrawTask * const task = xglCreateDrawTask(vert_array, color_array, index_array, allocator);
  task->task_type = TT_POLYLINE;

  Array_reset(vert_array, nullptr);
  Array_reset(color_array, nullptr);
  Array_reset(index_array, nullptr);
  Array_destroy(vert_array);
  Array_destroy(color_array);
  Array_destroy(index_array);

  return task;
}

DrawTask *xglCreatePixelPolyline(PixelVertex *vertices, int count, int plane_index, bool cycle,
                                 const Allocator *allocator) {
  Array *vertex_array = Array_new(sizeof(XGLCoord), allocator);
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
    Array_append(vertex_array, vertex, 1);
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

  DrawTask * const task = xglCreateDrawTask(vertex_array, color_array, index_array, allocator);
  task->task_type = TT_POLYLINE;

  Array_reset(vertex_array, nullptr);
  Array_reset(color_array, nullptr);
  Array_reset(index_array, nullptr);
  Array_destroy(vertex_array);
  Array_destroy(color_array);
  Array_destroy(index_array);

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
