/**
 * Project Name: xide
 * Module Name: runtime
 * Filename: drawUI.c
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
#include <stdlib.h>

DrawTask *TASK;
iXGLshProg BUILTIN_GRADUAL_SHADER_PROGRAM;

DrawTask *xglCreateDrawTask(Array *vertex_array, Array *color_array, Array *index_array,
                            MainWindow *window) {
  iXGLVao VAO = {};
  glCreateVertexArrays(1, &VAO);
  glEnableVertexArrayAttrib(VAO, LOC_VERTEX);
  glEnableVertexArrayAttrib(VAO, LOC_COLOR);
  glEnableVertexArrayAttrib(VAO, LOC_WINDOW_SIZE);

  iXGLVbo VBOs[3] = {};
  glCreateBuffers(3, VBOs);
  glNamedBufferStorage(VBOs[0], Array_length(vertex_array) * (GLsizeiptr) sizeof(XGLVertex),
                       Array_get(vertex_array, 0), 0);
  glNamedBufferStorage(VBOs[1], Array_length(color_array) * (GLsizeiptr) sizeof(XGLColor),
                       Array_get(color_array, 0), 0);
  glNamedBufferStorage(VBOs[2], Array_length(index_array) * (GLsizeiptr) sizeof(GLint),
                       Array_get(index_array, 0), 0);

  glVertexArrayAttribBinding(VAO, LOC_VERTEX, 0);
  glVertexArrayAttribFormat(VAO, LOC_VERTEX, sizeof(XGLVertex) / sizeof(GLfloat), GL_FLOAT,
                            GL_FALSE, 0);
  glVertexArrayVertexBuffer(VAO, 0, VBOs[0], 0, sizeof(XGLVertex));

  glVertexArrayAttribBinding(VAO, LOC_COLOR, 1);
  glVertexArrayAttribFormat(VAO, LOC_COLOR, sizeof(XGLColor) / sizeof(GLfloat), GL_FLOAT, GL_FALSE,
                            0);
  glVertexArrayVertexBuffer(VAO, 1, VBOs[1], 0, sizeof(XGLColor));

  glVertexArrayElementBuffer(VAO, VBOs[2]);

  DrawTask *task = window->allocator->calloc(1, sizeof(DrawTask));
  task->VAO = VAO;
  task->VBOs = Array_new(sizeof(iXGLVbo), window->allocator);
  task->IBO = VBOs[2];
  task->n_index = (GLsizei) Array_length(index_array);
  task->uniforms = Array_new(sizeof(iXGLVUniform), window->allocator);
  task->program = BUILTIN_GRADUAL_SHADER_PROGRAM;

  Array_append(task->VBOs, VBOs, 2);
  iXGLVUniform uniform = {uniform_type(US_2SCA, UD_INT), LOC_WINDOW_SIZE};
  Array_append(task->uniforms, &uniform, 1);

  return task;
}

void xglDestroyDrawTask(DrawTask *task) {
  const iXGLVbo *buffer = (iXGLVbo *) Array_get(task->VBOs, 0);
  glDeleteBuffers((GLint) Array_length(task->VBOs), buffer);
  glDeleteVertexArrays(1, &task->VAO);

  Array_reset(task->VBOs, nullptr);
  Array_reset(task->uniforms, nullptr);
  Array_destroy(task->VBOs);
  Array_destroy(task->uniforms);
}

DrawTask *xglCreateLines(Line *lines, int count, int plane_index, MainWindow *window) {
  Array *vertex_array = Array_new(sizeof(XGLVertex), window->allocator);
  Array *color_array = Array_new(sizeof(XGLColor), window->allocator);
  Array *index_array = Array_new(sizeof(GLint), window->allocator);
  for (int i = 0; i < count; i++) {
    XGLVertex vertices[2] = {};
    XGLColor colors[2] = {};
    GLint indices[2] = {2 * i, 2 * i + 1};
    rgba2XGLColor(lines[i].colors[0], &colors[0]);
    rgba2XGLColor(lines[i].colors[1], &colors[1]);
    vertices[VERTEX_BEGIN][AXIS_X] = (float) lines[i].start[AXIS_X];
    vertices[VERTEX_BEGIN][AXIS_Y] = (float) lines[i].start[AXIS_Y];
    vertices[VERTEX_BEGIN][AXIS_Z] = (float) (plane_index % 100) / 100.0f;
    vertices[VERTEX_BEGIN][AXIS_W] = 0.0f;
    vertices[VERTEX_END][AXIS_X] = (float) lines[i].end[AXIS_X];
    vertices[VERTEX_END][AXIS_Y] = (float) lines[i].end[AXIS_Y];
    vertices[VERTEX_END][AXIS_Z] = (float) (plane_index % 100) / 100.0f;
    vertices[VERTEX_END][AXIS_W] = 0.0f;
    Array_append(vertex_array, vertices, 2);
    Array_append(color_array, colors, 2);
    Array_append(index_array, indices, 2);
  }

  DrawTask *task = xglCreateDrawTask(vertex_array, color_array, index_array, window);

  Array_reset(vertex_array, nullptr);
  Array_reset(color_array, nullptr);
  Array_reset(index_array, nullptr);
  Array_destroy(vertex_array);
  Array_destroy(color_array);
  Array_destroy(index_array);

  return task;
}

DrawTask *xglCreateSolidPolygon(Vertex *vertices, int count, int plane_index, MainWindow *window) {
  Array *vertex_array = Array_new(sizeof(XGLVertex), window->allocator);
  Array *color_array = Array_new(sizeof(XGLColor), window->allocator);
  for (int i = 0; i < count; i++) {
    XGLVertex vertex = {};
    XGLColor color = {};
    rgba2XGLColor(vertices[i].color, &color);
    vertex[AXIS_X] = (float) vertices[i].coord[AXIS_X];
    vertex[AXIS_Y] = (float) vertices[i].coord[AXIS_Y];
    vertex[AXIS_Z] = (float) (plane_index % 100) / 100.0f;
    vertex[AXIS_W] = 0.0f;
    Array_append(vertex_array, vertex, 1);
    Array_append(color_array, color, 1);
  }
  Array *coord_array = Array_new(sizeof(float[2]), window->allocator);
  for (int i = 0; i < count; i++) {
    const float coord[2] = {(float) vertices[i].coord[AXIS_X], (float) vertices[i].coord[AXIS_Y]};
    Array_append(coord_array, coord, 1);
  }
  Array *index_array = xglCreateDelaunayIndexArray(coord_array, window->allocator);
  Array_reset(coord_array, nullptr);
  Array_destroy(coord_array);

  DrawTask *task = xglCreateDrawTask(vertex_array, color_array, index_array, window);

  Array_reset(vertex_array, nullptr);
  Array_reset(color_array, nullptr);
  Array_reset(index_array, nullptr);
  Array_destroy(vertex_array);
  Array_destroy(color_array);
  Array_destroy(index_array);

  return task;
}

DrawTask *xglCreatePolyline(Vertex *vertices, int count, bool cycle, int plane_index,
                            MainWindow *window) {
  Array *vertex_array = Array_new(sizeof(XGLVertex), window->allocator);
  Array *color_array = Array_new(sizeof(XGLColor), window->allocator);
  Array *index_array = Array_new(sizeof(GLint), window->allocator);
  for (int i = 0; i < count; i++) {
    XGLVertex vertex = {};
    XGLColor color = {};
    rgba2XGLColor(vertices[i].color, &color);
    vertex[AXIS_X] = (float) vertices[i].coord[AXIS_X];
    vertex[AXIS_Y] = (float) vertices[i].coord[AXIS_Y];
    vertex[AXIS_Z] = (float) (plane_index % 100) / 100.0f;
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

  DrawTask *task = xglCreateDrawTask(vertex_array, color_array, index_array, window);

  Array_reset(vertex_array, nullptr);
  Array_reset(color_array, nullptr);
  Array_reset(index_array, nullptr);
  Array_destroy(vertex_array);
  Array_destroy(color_array);
  Array_destroy(index_array);

  return task;
}

void xglDrawLines(DrawTask *task, const GLfloat windowSize[2]) {
  glUseProgram(task->program);
  glBindVertexArray(task->VAO);
  for (uint32_t i = 0; i < Array_length(task->uniforms); i++) {
    iXGLVUniform *uniform = (iXGLVUniform *) Array_get(task->uniforms, i);
    glProgramUniform2fv(task->program, uniform->u_locate, 1, windowSize);
  }
  glDrawElements(GL_LINES, task->n_index, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void xglDrawSolidArea(DrawTask *task, const GLfloat windowSize[2]) {
  glUseProgram(task->program);
  glBindVertexArray(task->VAO);
  //    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  for (uint32_t i = 0; i < Array_length(task->uniforms); i++) {
    iXGLVUniform *uniform = (iXGLVUniform *) Array_get(task->uniforms, i);
    glProgramUniform2fv(task->program, uniform->u_locate, 1, windowSize);
  }
  glDrawElements(GL_TRIANGLES, task->n_index, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void xglDrawPolyline(DrawTask *task, const GLfloat windowSize[2]) {
  glUseProgram(task->program);
  glBindVertexArray(task->VAO);
  for (uint32_t i = 0; i < Array_length(task->uniforms); i++) {
    iXGLVUniform *uniform = (iXGLVUniform *) Array_get(task->uniforms, i);
    glProgramUniform2fv(task->program, uniform->u_locate, 1, windowSize);
  }
  glDrawElements(GL_LINE_STRIP, task->n_index, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
