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
 * Filename: draw.c
 * Creator: Yaokai Liu
 * Create Date: 2024-7-7
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "draw.h"
#include "GLFW/glfw3.h"
#include "cg2d.h"
#include "glad/glad.h"
#include "print.h"
#include "utils.h"
#include "widgets.h"
#include "xgl-object.h"

#define xglInitSingleAttrVBO(loc, data_array, data_type, normed)                                       \
  do {                                                                                                 \
    const GLsizeiptr total_size = Array_length((data_array)) * (GLsizeiptr) sizeof(data_type);         \
    const GLvoid *address = Array_real_addr((data_array), 0);                                          \
    glNamedBufferStorage(VBOs[(loc)], total_size, address, 0);                                         \
    glVertexArrayAttribBinding(VAO, (loc), (loc));                                                     \
    glVertexArrayAttribFormat(VAO, (loc), sizeof(data_type) / sizeof(GLfloat), GL_FLOAT, (normed), 0); \
    glVertexArrayVertexBuffer(VAO, (loc), VBOs[(loc)], 0, sizeof(data_type));                          \
  } while (false)

inline DrawTask *xglCreateNonTextureDrawTask(const Array * const vertex_array, const Array * const color_array,
                                             const Array * const index_array, const Allocator * const allocator) {
  iXGLVao VAO = {};
  glCreateVertexArrays(1, &VAO);
  glEnableVertexArrayAttrib(VAO, LOC_COORD);
  glEnableVertexArrayAttrib(VAO, LOC_COLOR);
  glEnableVertexArrayAttrib(VAO, LOC_VIEWPORT);

  iXGLVbo VBOs[2] = {};
  glCreateBuffers(2, VBOs);
  xglInitSingleAttrVBO(LOC_COORD, vertex_array, XGLCoord, GL_FALSE);
  xglInitSingleAttrVBO(LOC_COLOR, color_array, XGLColor, GL_FALSE);

  iXGLVbo IBO = 0;
  glCreateBuffers(1, &IBO);
  const GLsizeiptr total_size = Array_length(index_array) * (GLsizeiptr) sizeof(GLint);
  const GLvoid *address = Array_real_addr(index_array, 0);
  glNamedBufferStorage(IBO, total_size, address, 0);
  glVertexArrayElementBuffer(VAO, IBO);

  DrawTask *task = allocator->calloc(1, sizeof(DrawTask));
  task->VAO = VAO;
  task->VBOs = Array_new(sizeof(iXGLVbo), enum_XGL_VBO, allocator);
  task->IBO = IBO;
  task->n_index = (GLsizei) Array_length(index_array);
  task->uniforms = Array_new(sizeof(iXGLVUniform), enum_XGL_UNIFORM, allocator);

  Array_append(task->VBOs, VBOs, 2);
  iXGLVUniform uniform = {uniform_type(US_4SCA, UD_INT), LOC_VIEWPORT};
  Array_append(task->uniforms, &uniform, 1);

  return task;
}

inline DrawTask *
xglCreateTexturedDrawTask(const Array *const vertex_array, const Array *index_array, const Allocator *const allocator) {
  iXGLVao VAO = {};
  glCreateVertexArrays(1, &VAO);
  glEnableVertexArrayAttrib(VAO, LOC_COORD);
  glEnableVertexArrayAttrib(VAO, LOC_COLOR);
  glEnableVertexArrayAttrib(VAO, LOC_TEXTURE_COORD);
  glEnableVertexArrayAttrib(VAO, LOC_VIEWPORT);

  iXGLVbo VBO = 0;
  glCreateBuffers(1, &VBO);
  glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(XGLVertex));

  GLsizeiptr total_size = Array_length((vertex_array)) * (GLsizeiptr) sizeof(XGLVertex);
  const GLvoid *address = Array_real_addr((vertex_array), 0);
  glNamedBufferStorage(VBO, total_size, address, 0);
  glVertexArrayAttribBinding(VAO, LOC_COORD, 0);
  glVertexArrayAttribFormat(VAO, LOC_COORD, sizeof(XGLCoord) / sizeof(GLfloat),
                            GL_FLOAT, GL_FALSE, offsetof(XGLVertex, coord));
  glVertexArrayAttribBinding(VAO, LOC_COLOR, 0);
  glVertexArrayAttribFormat(VAO, LOC_COLOR, sizeof(XGLColor) / sizeof(GLfloat),
                            GL_FLOAT, GL_FALSE, offsetof(XGLVertex, color));
  glVertexArrayAttribBinding(VAO, LOC_TEXTURE_COORD, 0);
  glVertexArrayAttribFormat(VAO, LOC_TEXTURE_COORD, sizeof(XGLTexCoord) / sizeof(GLfloat),
                            GL_FLOAT, GL_TRUE, offsetof(XGLVertex, tex_coord));

  iXGLVbo IBO = 0;
  glCreateBuffers(1, &IBO);
  total_size = Array_length(index_array) * (GLsizeiptr) sizeof(GLint);
  address = Array_real_addr(index_array, 0);
  glNamedBufferStorage(IBO, total_size, address, 0);
  glVertexArrayElementBuffer(VAO, IBO);

  DrawTask *task = allocator->calloc(1, sizeof(DrawTask));
  task->VAO = VAO;
  task->VBOs = Array_new(sizeof(iXGLVbo), enum_XGL_VBO, allocator);
  task->IBO = IBO;
  task->n_index = (GLsizei) Array_length(index_array);
  task->uniforms = Array_new(sizeof(iXGLVUniform), enum_XGL_UNIFORM, allocator);

  Array_append(task->VBOs, &VBO, 1);
  iXGLVUniform uniforms[] = {
    {uniform_type(US_4SCA, UD_INT), LOC_VIEWPORT    },
    {uniform_type(US_1SCA, UD_INT), LOC_TEXTURE_UNIT}
  };
  Array_append(task->uniforms, uniforms, 1);

  return task;
}

inline void xglDestroyDrawTask(DrawTask * const task, const Allocator *) {
  const iXGLVbo *buffer = (iXGLVbo *) Array_real_addr(task->VBOs, 0);
  glDeleteBuffers((GLint) Array_length(task->VBOs), buffer);
  glDeleteVertexArrays(1, &task->VAO);

  releasePrimeArray(task->VBOs);
  releasePrimeArray(task->uniforms);
}

inline void xglBindShaderProgram(DrawTask *task, GLuint program) {
  task->program = program;
}

DrawTask *xglCreatePixelLines(const Array * const line_array, const int plane_index,
                              const Allocator * const allocator) {
  const int count = (int) Array_length(line_array);
  const Line * const lines = Array_real_addr(line_array, 0);
  Array *vertex_array = Array_new(sizeof(XGLCoord), enum_XGL_COORD, allocator);
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

  DrawTask * const task = xglCreateNonTextureDrawTask(vertex_array, color_array, index_array, allocator);
  task->task_type = TT_LINES;

  releasePrimeArray(vertex_array);
  releasePrimeArray(color_array);
  releasePrimeArray(index_array);

  return task;
}

DrawTask *xglCreatePolygon2D(const Array * const vertex_array, const float plane_index, const bool solid,
                             const Allocator * const allocator) {
  const int count = (int) Array_length(vertex_array);
  const Vertex2D * const vertices = Array_real_addr(vertex_array, 0);
  Array *coord_array = Array_new(sizeof(XGLCoord), enum_XGL_COORD, allocator);
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

  DrawTask * const task = xglCreateNonTextureDrawTask(coord_array, color_array, index_array, allocator);
  task->task_type = solid ? TT_SOLID_AREA : TT_TRIANGULATED_AREA;

  releasePrimeArray(coord_array);
  releasePrimeArray(color_array);
  releasePrimeArray(index_array);

  return task;
}

DrawTask *xglCreateCurveArea2D(const Array * const vertex_array, const float plane_index, const bool cycle,
                               const bool solid, const Allocator * const allocator) {
  const int count = (int) Array_length(vertex_array);
  const Vertex2D * const vertices = Array_real_addr(vertex_array, 0);
  Array *coord_array = Array_new(sizeof(XGLCoord), enum_XGL_COORD, allocator);
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

  DrawTask * const task = xglCreateNonTextureDrawTask(coord_array, color_array, index_array, allocator);
  task->task_type = solid ? TT_SOLID_AREA : TT_TRIANGULATED_AREA;

  releasePrimeArray(coord_array);
  releasePrimeArray(color_array);
  releasePrimeArray(index_array);

  return task;
}

DrawTask *xglCreatePixelPolygon2D(const Array * const vertex_array, int plane_index, bool solid,
                                  const Allocator *allocator) {
  const int count = (int) Array_length(vertex_array);
  const Vertex2D * const vertices = Array_real_addr(vertex_array, 0);
  Array *coord_array = Array_new(sizeof(XGLCoord), enum_XGL_COORD, allocator);
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

  DrawTask * const task = xglCreateNonTextureDrawTask(coord_array, color_array, index_array, allocator);
  task->task_type = solid ? TT_SOLID_AREA : TT_TRIANGULATED_AREA;

  releasePrimeArray(coord_array);
  releasePrimeArray(color_array);
  releasePrimeArray(index_array);

  return task;
}

DrawTask *xglCreatePolyline2D(const Array * const vertex_array, const float plane_index, const bool cycle,
                              const Allocator * const allocator) {
  const int count = (int) Array_length(vertex_array);
  const Vertex2D * const vertices = Array_real_addr(vertex_array, 0);
  Array *coord_array = Array_new(sizeof(XGLCoord), enum_XGL_COORD, allocator);
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

  DrawTask * const task = xglCreateNonTextureDrawTask(coord_array, color_array, index_array, allocator);
  task->task_type = TT_POLYLINE;

  releasePrimeArray(coord_array);
  releasePrimeArray(color_array);
  releasePrimeArray(index_array);

  return task;
}

DrawTask *xglCreatePixelPolyline2D(const Array * const vertex_array, int plane_index, bool cycle,
                                   const Allocator *allocator) {
  const int count = (int) Array_length(vertex_array);
  const Vertex2D * const vertices = Array_real_addr(vertex_array, 0);
  Array *coord_array = Array_new(sizeof(XGLCoord), enum_XGL_COORD, allocator);
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

  DrawTask * const task = xglCreateNonTextureDrawTask(coord_array, color_array, index_array, allocator);
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
  glProgramUniform4fv(task->program, location, 1, viewport);
  glDrawElements(GL_LINES, task->n_index, GL_UNSIGNED_INT, nullptr);
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
  glProgramUniform4fv(task->program, location, 1, viewport);
  glDrawElements(GL_TRIANGLES, task->n_index, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

inline void xglDrawPolyline(const DrawTask * const task, const GLfloat viewport[4]) {
  glUseProgram(task->program);
  glBindVertexArray(task->VAO);
  int location = glGetUniformLocation(task->program, "viewport");
  glProgramUniform4fv(task->program, location, 1, viewport);
  glDrawElements(GL_LINE_STRIP, task->n_index, GL_UNSIGNED_INT, nullptr);
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
    case TT_TEXT: {
      return xglPrintText(task, viewport);
    }
  }
}
