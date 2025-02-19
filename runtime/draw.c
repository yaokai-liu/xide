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
                       Array_real_addr(vertex_array, 0), 0);
  glNamedBufferStorage(VBOs[1], Array_length(color_array) * (GLsizeiptr) sizeof(XGLColor),
                       Array_real_addr(color_array, 0), 0);
  glNamedBufferStorage(VBOs[2], Array_length(index_array) * (GLsizeiptr) sizeof(GLint),
                       Array_real_addr(index_array, 0), 0);

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
  task->VBOs = Array_new(sizeof(iXGLVbo), enum_XGL_VBO, allocator);
  task->IBO = VBOs[2];
  task->n_index = (GLsizei) Array_length(index_array);
  task->uniforms = Array_new(sizeof(iXGLVUniform), enum_XGL_UNIFORM, allocator);

  Array_append(task->VBOs, VBOs, 2);
  iXGLVUniform uniform = {uniform_type(US_4SCA, UD_INT), LOC_WINDOW_SIZE};
  Array_append(task->uniforms, &uniform, 1);

  return task;
}

inline void xglDestroyDrawTask(DrawTask * const task, const Allocator *) {
  const iXGLVbo *buffer = (iXGLVbo *) Array_real_addr(task->VBOs, 0);
  glDeleteBuffers((GLint) Array_length(task->VBOs), buffer);
  glDeleteVertexArrays(1, &task->VAO);

  releasePrimeArray(task->VBOs);
  releasePrimeArray(task->uniforms);
}

void xglBindShaderProgram(DrawTask *task, GLuint program) {
  task->program = program;
}

DrawTask *xglCreatePixelLines(const Array * const line_array, const int plane_index,
                              const Allocator * const allocator) {
  const int count = (int) Array_length(line_array);
  const Line * const lines = Array_real_addr(line_array, 0);
  Array *vertex_array = Array_new(sizeof(XGLCoord), enum_XGL_VERTEX, allocator);
  Array *color_array = Array_new(sizeof(XGLColor), enum_XGL_COLOR, allocator);
  Array *index_array = Array_new(sizeof(GLint), enum_XGL_INDEX, allocator);
  for (int i = 0; i < count; i++) {
    XGLCoord vertices[2] = {};
    XGLColor colors[2] = {};
    GLint indices[2] = {2 * i, 2 * i + 1};
    rgba2XGLColor(lines[i][0].color, &colors[0]);
    rgba2XGLColor(lines[i][1].color, &colors[1]);
    vertices[VERTEX_BEGIN][AXIS_X] = (float) lines[i][0].coord[AXIS_X];
    vertices[VERTEX_BEGIN][AXIS_Y] = (float) lines[i][0].coord[AXIS_Y];
    vertices[VERTEX_BEGIN][AXIS_Z] = (float) plane_index;
    vertices[VERTEX_BEGIN][AXIS_W] = 0.0f;
    vertices[VERTEX_END][AXIS_X] = (float) lines[i][1].coord[AXIS_X];
    vertices[VERTEX_END][AXIS_Y] = (float) lines[i][1].coord[AXIS_Y];
    vertices[VERTEX_END][AXIS_Z] = (float) plane_index;
    vertices[VERTEX_END][AXIS_W] = 0.0f;
    Array_append(vertex_array, vertices, 2);
    Array_append(color_array, colors, 2);
    Array_append(index_array, indices, 2);
  }

  DrawTask * const task = xglCreateDrawTask(vertex_array, color_array, index_array, allocator);
  task->task_type = TT_LINES;

  releasePrimeArray(vertex_array);
  releasePrimeArray(color_array);
  releasePrimeArray(index_array);

  return task;
}

DrawTask *xglCreatePolygon2D(const Array * const vertex_array, const float plane_index,
                             const bool solid, const Allocator * const allocator) {
  const int count = (int) Array_length(vertex_array);
  const Vertex * const vertices = Array_real_addr(vertex_array, 0);
  Array *coord_array = Array_new(sizeof(XGLCoord), enum_XGL_VERTEX, allocator);
  Array *color_array = Array_new(sizeof(XGLColor), enum_XGL_COLOR, allocator);
  for (int i = 0; i < count; i++) {
    XGLCoord vertex = {};
    XGLColor color = {};
    rgba2XGLColor(vertices[i].color, &color);
    vertex[AXIS_X] = vertices[i].coord[AXIS_X];
    vertex[AXIS_Y] = vertices[i].coord[AXIS_Y];
    vertex[AXIS_Z] = plane_index;
    vertex[AXIS_W] = 0.0f;
    Array_append(coord_array, vertex, 1);
    Array_append(color_array, color, 1);
  }
  Array *index_array = xglEarClippingTriangulate2D(coord_array, allocator);

  DrawTask * const task = xglCreateDrawTask(coord_array, color_array, index_array, allocator);
  task->task_type = solid ? TT_SOLID_AREA : TT_TRIANGULATED_AREA;

  releasePrimeArray(coord_array);
  releasePrimeArray(color_array);
  releasePrimeArray(index_array);

  return task;
}

DrawTask *xglCreateCurveArea2D(const Array * const vertex_array, const float plane_index,
                               const bool cycle, const bool solid,
                               const Allocator * const allocator) {
  const int count = (int) Array_length(vertex_array);
  const Vertex * const vertices = Array_real_addr(vertex_array, 0);
  Array *coord_array = Array_new(sizeof(XGLCoord), enum_XGL_VERTEX, allocator);
  Array *color_array = Array_new(sizeof(XGLColor), enum_XGL_COLOR, allocator);
  for (int i = 0; i < count; i++) {
    XGLCoord vertex = {};
    XGLColor color = {};
    rgba2XGLColor(vertices[i].color, &color);
    vertex[AXIS_X] = vertices[i].coord[AXIS_X];
    vertex[AXIS_Y] = vertices[i].coord[AXIS_Y];
    vertex[AXIS_Z] = plane_index;
    vertex[AXIS_W] = 0.0f;
    Array_append(coord_array, vertex, 1);
    Array_append(color_array, color, 1);
  }
  Array *index_array = xglRadialTriangulation2D(coord_array, cycle, allocator);

  DrawTask * const task = xglCreateDrawTask(coord_array, color_array, index_array, allocator);
  task->task_type = solid ? TT_SOLID_AREA : TT_TRIANGULATED_AREA;

  releasePrimeArray(coord_array);
  releasePrimeArray(color_array);
  releasePrimeArray(index_array);

  return task;
}

DrawTask *xglCreatePixelPolygon(const Array * const vertex_array, int plane_index, bool solid,
                                const Allocator *allocator) {
  const int count = (int) Array_length(vertex_array);
  const Vertex * const vertices = Array_real_addr(vertex_array, 0);
  Array *coord_array = Array_new(sizeof(XGLCoord), enum_XGL_VERTEX, allocator);
  Array *color_array = Array_new(sizeof(XGLColor), enum_XGL_COLOR, allocator);
  for (int i = 0; i < count; i++) {
    XGLCoord vertex = {};
    XGLColor color = {};
    rgba2XGLColor(vertices[i].color, &color);
    vertex[AXIS_X] = (float) vertices[i].coord[AXIS_X];
    vertex[AXIS_Y] = (float) vertices[i].coord[AXIS_Y];
    vertex[AXIS_Z] = (float) plane_index;
    vertex[AXIS_W] = 0.0f;
    Array_append(coord_array, vertex, 1);
    Array_append(color_array, color, 1);
  }
  Array *index_array = xglEarClippingTriangulate2D(coord_array, allocator);

  DrawTask * const task = xglCreateDrawTask(coord_array, color_array, index_array, allocator);
  task->task_type = solid ? TT_SOLID_AREA : TT_TRIANGULATED_AREA;

  releasePrimeArray(coord_array);
  releasePrimeArray(color_array);
  releasePrimeArray(index_array);

  return task;
}

DrawTask *xglCreatePolyline2D(const Array * const vertex_array, const float plane_index,
                              const bool cycle, const Allocator * const allocator) {
  const int count = (int) Array_length(vertex_array);
  const Vertex * const vertices = Array_real_addr(vertex_array, 0);
  Array *coord_array = Array_new(sizeof(XGLCoord), enum_XGL_VERTEX, allocator);
  Array *color_array = Array_new(sizeof(XGLColor), enum_XGL_COLOR, allocator);
  Array *index_array = Array_new(sizeof(GLint), enum_XGL_INDEX, allocator);
  for (int i = 0; i < count; i++) {
    XGLCoord vertex = {};
    XGLColor color = {};
    rgba2XGLColor(vertices[i].color, &color);
    vertex[AXIS_X] = vertices[i].coord[AXIS_X];
    vertex[AXIS_Y] = vertices[i].coord[AXIS_Y];
    vertex[AXIS_Z] = plane_index;
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

  releasePrimeArray(coord_array);
  releasePrimeArray(color_array);
  releasePrimeArray(index_array);

  return task;
}

DrawTask *xglCreatePixelPolyline(const Array * const vertex_array, int plane_index, bool cycle,
                                 const Allocator *allocator) {
  const int count = (int) Array_length(vertex_array);
  const Vertex * const vertices = Array_real_addr(vertex_array, 0);
  Array *coord_array = Array_new(sizeof(XGLCoord), enum_XGL_VERTEX, allocator);
  Array *color_array = Array_new(sizeof(XGLColor), enum_XGL_COLOR, allocator);
  Array *index_array = Array_new(sizeof(GLint), enum_XGL_INDEX, allocator);
  for (int i = 0; i < count; i++) {
    XGLCoord vertex = {};
    XGLColor color = {};
    rgba2XGLColor(vertices[i].color, &color);
    vertex[AXIS_X] = (float) vertices[i].coord[AXIS_X];
    vertex[AXIS_Y] = (float) vertices[i].coord[AXIS_Y];
    vertex[AXIS_Z] = (float) plane_index;
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

  releasePrimeArray(coord_array);
  releasePrimeArray(color_array);
  releasePrimeArray(index_array);

  return task;
}

inline void xglDrawLines(const DrawTask * const task, const GLfloat viewport[4]) {
  glUseProgram(task->program);
  glBindVertexArray(task->VAO);
  int location = glGetUniformLocation(task->program, "viewport");
  const int n_uniforms = (int) Array_length(task->uniforms);
  const iXGLVUniform * const uniforms = Array_real_addr(task->uniforms, 0);
  for (uint32_t i = 0; i < n_uniforms; i++) {
    const iXGLVUniform * const uniform = &uniforms[i];
    glProgramUniform4fv(task->program, location, 1, viewport);
  }
  glDrawElements(GL_LINES, task->n_index, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

inline void xglDrawArea(const DrawTask * const task, const GLfloat viewport[4]) {
  glUseProgram(task->program);
  glBindVertexArray(task->VAO);
  if (task->task_type == TT_SOLID_AREA) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  int location = glGetUniformLocation(task->program, "viewport");
  const int n_uniforms = (int) Array_length(task->uniforms);
  const iXGLVUniform * const uniforms = Array_real_addr(task->uniforms, 0);
  for (uint32_t i = 0; i < n_uniforms; i++) {
    const iXGLVUniform * const uniform = &uniforms[i];
    glProgramUniform4fv(task->program, location, 1, viewport);
  }
  glDrawElements(GL_TRIANGLES, task->n_index, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

inline void xglDrawPolyline(const DrawTask * const task, const GLfloat viewport[4]) {
  glUseProgram(task->program);
  glBindVertexArray(task->VAO);
  int location = glGetUniformLocation(task->program, "viewport");
  const int n_uniforms = (int) Array_length(task->uniforms);
  const iXGLVUniform * const uniforms = Array_real_addr(task->uniforms, 0);
  for (uint32_t i = 0; i < n_uniforms; i++) {
    const iXGLVUniform * const uniform = &uniforms[i];
    glProgramUniform4fv(task->program, location, 1, viewport);
  }
  glDrawElements(GL_LINE_STRIP, task->n_index, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

inline void xglDraw(const DrawTask * const task, const IdeWindow * const window) {
  const float * const viewport = (const float *) &window->info.viewport;
  switch (task->task_type) {
    case TT_LINES: {
      return xglDrawLines(task, viewport);
    }
    case TT_POLYLINE: {
      return xglDrawPolyline(task, viewport);
    }
    case TT_SOLID_AREA:
    case TT_TRIANGULATED_AREA: {
      return xglDrawArea(task, viewport);
    }
  }
}
