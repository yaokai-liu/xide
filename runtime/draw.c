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

#include "minmax.h"
#include "draw.h"
#include "GLFW/glfw3.h"
#include "cg2d.h"
#include "glad/glad.h"
#include "print.h"
#include "utils.h"
#include "widgets.h"
#include "xgl-object.h"
#include "enum.h"

#define xglInitSingleAttrVBO(loc, data_array, data_type, normed)                                       \
  do {                                                                                                 \
    const GLsizeiptr total_size = Array_length((data_array)) * (GLsizeiptr) sizeof(data_type);         \
    const GLvoid *address = Array_real_addr((data_array), 0);                                          \
    glNamedBufferStorage(VBOs[(loc)], total_size, address, 0);                                         \
    glVertexArrayAttribBinding(VAO, (loc), (loc));                                                     \
    glVertexArrayAttribFormat(VAO, (loc), sizeof(data_type) / sizeof(GLfloat), GL_FLOAT, (normed), 0); \
    glVertexArrayVertexBuffer(VAO, (loc), VBOs[(loc)], 0, sizeof(data_type));                          \
  } while (false)

inline DrawTask *ideCreateNonTextureDrawTask(const Array * const vertex_array, const Array * const color_array,
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
ideCreateTexturedDrawTask(const Array *const vertex_array, const Array *index_array, const TextureAtlas *atlas,
                          const Allocator *const allocator) {
  iXGLVao VAO = {};
  glCreateVertexArrays(1, &VAO);
  glEnableVertexArrayAttrib(VAO, LOC_COORD);
  glEnableVertexArrayAttrib(VAO, LOC_COLOR);
  glEnableVertexArrayAttrib(VAO, LOC_TEXTURE_COORD);
  glEnableVertexArrayAttrib(VAO, LOC_VIEWPORT);
  glEnableVertexArrayAttrib(VAO, LOC_TEXTURE_ATLAS_SIZE);

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
    {uniform_type(US_2SCA, UD_FLOAT), LOC_TEXTURE_ATLAS_SIZE},
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

DrawTask *ideCreatePixelLines(const Array * const line_array, const int plane_index,
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

  DrawTask * const task = ideCreateNonTextureDrawTask(vertex_array, color_array, index_array, allocator);
  task->task_type = TT_LINES;

  releasePrimeArray(vertex_array);
  releasePrimeArray(color_array);
  releasePrimeArray(index_array);

  return task;
}

DrawTask *ideCreatePolygon2D(const Array * const vertex_array, const float plane_index, const bool solid,
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

  DrawTask * const task = ideCreateNonTextureDrawTask(coord_array, color_array, index_array, allocator);
  task->task_type = solid ? TT_SOLID_AREA : TT_TRIANGULATED_AREA;

  releasePrimeArray(coord_array);
  releasePrimeArray(color_array);
  releasePrimeArray(index_array);

  return task;
}

DrawTask *ideCreateCurveArea2D(const Array * const vertex_array, const float plane_index, const bool cycle,
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

  DrawTask * const task = ideCreateNonTextureDrawTask(coord_array, color_array, index_array, allocator);
  task->task_type = solid ? TT_SOLID_AREA : TT_TRIANGULATED_AREA;

  releasePrimeArray(coord_array);
  releasePrimeArray(color_array);
  releasePrimeArray(index_array);

  return task;
}

DrawTask *ideCreatePixelPolygon2D(const Array * const vertex_array, int plane_index, bool solid,
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

  DrawTask * const task = ideCreateNonTextureDrawTask(coord_array, color_array, index_array, allocator);
  task->task_type = solid ? TT_SOLID_AREA : TT_TRIANGULATED_AREA;

  releasePrimeArray(coord_array);
  releasePrimeArray(color_array);
  releasePrimeArray(index_array);

  return task;
}

DrawTask *ideCreatePolyline2D(const Array * const vertex_array, const float plane_index, const bool cycle,
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

  DrawTask * const task = ideCreateNonTextureDrawTask(coord_array, color_array, index_array, allocator);
  task->task_type = TT_POLYLINE;

  releasePrimeArray(coord_array);
  releasePrimeArray(color_array);
  releasePrimeArray(index_array);

  return task;
}

DrawTask *ideCreatePixelPolyline2D(const Array * const vertex_array, int plane_index, bool cycle,
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

  DrawTask * const task = ideCreateNonTextureDrawTask(coord_array, color_array, index_array, allocator);
  task->task_type = TT_POLYLINE;

  releasePrimeArray(coord_array);
  releasePrimeArray(color_array);
  releasePrimeArray(index_array);

  return task;
}

#define lenof(array) (sizeof(array) / sizeof(typeof((array)[0])))
inline DrawTask *ideCreateText2DByArray(IDE *ide, const Array /*<char_t>*/ *char_array, const Array /*<Vertex2D>*/ *vert_array,
                                       uint32_t plane_index, const Font *font) {
  if (!ide || !char_array || !vert_array || !font) { return nullptr; }
  const CharModelSet *set = ideUpdateCharModelSet(ide, font, char_array);
  return ideCreateDrawTextTask(ide, char_array, vert_array, set, plane_index, font);
}
inline DrawTask *
ideCreateDrawTextTask(IDE *ide, const Array *char_array, const Array *vert_array, const CharModelSet *set,
                      uint32_t plane_index, const Font *font) {
  if (!ide || !char_array || !vert_array || !set || !font) { return nullptr; }
  const Allocator *allocator = ide->allocator;
  TextureAtlas *atlas = Array_real_addr(ide->atlasManager, set->atlas - 1);
  Array *vertex_array = Array_new(sizeof(XGLVertex), enum_XGL_VERTEX, allocator);
  Array *index_array = Array_new(sizeof(GLuint), enum_XGL_INDEX, allocator);
  const uint32_t count = min(Array_length(vert_array), Array_length(char_array));
  if (!count) { return nullptr; }
  const Vertex2D * const pixel_vertices = Array_real_addr(vert_array, 0);
  const char_t * const string = Array_real_addr(char_array, 0);
  for (uint32_t i = 0; i < count; i++) {
    const CharModel *model = AVLTree_get(set->charTree, string[i]);
    model = Array_vert2real(set->modelArray, model);
    XGLVertex vertices[4] = {};
    xglGenCharCoord2D(model, &pixel_vertices[i], atlas, vertices);
    GLuint indices[6] = {
        i * 4 + RC_LT, i * 4 + RC_RT, i * 4 + RC_LB,
        i * 4 + RC_RT, i * 4 + RC_LB, i * 4 + RC_RB
    };
    Array_append(vertex_array, vertices, lenof(vertices));
    Array_append(index_array, indices, lenof(indices));
  }

  DrawTask * const task = ideCreateTexturedDrawTask(vertex_array, index_array, atlas, allocator);
  task->task_type = TT_TEXT;
  task->atlas_index = set->atlas - 1;
  task->texture_unit = atlas->unit;

  releasePrimeArray(vertex_array);
  releasePrimeArray(index_array);

  return task;
}

inline DrawTask *
ideCreateTextStr2D(IDE *ide, const Array *char_array, const Vertex2D *anchor,
                const float c_space, const uint32_t mode,
                uint32_t plane_index, const Font *font, XGLVector2D feedback_vec) {
  if (!ide || !char_array || !anchor || !font) { return nullptr; }
  const Allocator *allocator = ide->allocator;
  const CharModelSet *set = ideUpdateCharModelSet(ide, font, char_array);
  Array *vertex_array = nullptr;
  if ((mode & TS_D_MASK) == TS_VERTICAL) {
    vertex_array = charModelSetGenVCoordArray(set, char_array, anchor, c_space, mode, feedback_vec, allocator);
  } else {
    vertex_array = charModelSetGenHCoordArray(set, char_array, anchor, c_space, mode, feedback_vec, allocator);
  }
  DrawTask * const task = ideCreateDrawTextTask(ide, char_array, vertex_array, set, plane_index, font);
  if (vertex_array) { releasePrimeArray(vertex_array); }
  return task;
}

inline DrawTask *
ideCreateTextStr2DByStr(IDE *ide, const char_t *string, const Vertex2D *anchor,
                      float c_space, uint32_t mode,
                      uint32_t plane_index, const Font *font, XGLVector2D feedback_vec) {
  if (!ide || !string || !anchor || !font) { return nullptr; }
  const Allocator *allocator = ide->allocator;
  Array *char_array = Array_new(sizeof(char_t), enum_IDE_CHAR, allocator);
  Array_append(char_array, string, strlen(string));
  DrawTask * const task = ideCreateTextStr2D(ide, char_array, anchor, c_space, mode, plane_index, font, feedback_vec);
  releasePrimeArray(char_array);
  return task;
}

inline void ideDrawLines(const DrawTask * const task, const GLfloat viewport[4]) {
  glUseProgram(task->program);
  glBindVertexArray(task->VAO);
  int location = glGetUniformLocation(task->program, "viewport");
  glProgramUniform4fv(task->program, location, 1, viewport);
  glDrawElements(GL_LINES, task->n_index, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

inline void ideDrawArea(const DrawTask * const task, const GLfloat viewport[4]) {
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

inline void ideDrawPolyline(const DrawTask * const task, const GLfloat viewport[4]) {
  glUseProgram(task->program);
  glBindVertexArray(task->VAO);
  int location = glGetUniformLocation(task->program, "viewport");
  glProgramUniform4fv(task->program, location, 1, viewport);
  glDrawElements(GL_LINE_STRIP, task->n_index, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

void ideDrawText(IDE *ide, const DrawTask *task, const GLfloat viewport[2]) {
  const TextureAtlas *atlas = Array_real_addr(ide->atlasManager, task->atlas_index);
  const GLfloat atlas_size[2] = {[AXIS_X] = (float) atlas->width, [AXIS_Y] = (float) atlas->height};
  int loc_viewport = glGetUniformLocation(task->program, "viewport");
  int loc_atlas_size = glGetUniformLocation(task->program, "texSize");
  int loc_tex_unit = glGetUniformLocation(task->program, "tex");
  glUseProgram(task->program);
  glBindVertexArray(task->VAO);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBindTextureUnit(task->texture_unit, atlas->texture);
  glProgramUniform4fv(task->program, loc_viewport, 1, viewport);
  glProgramUniform2fv(task->program, loc_atlas_size, 1, atlas_size);
  glProgramUniform1i(task->program, loc_tex_unit, (GLint) task->texture_unit);
  glDrawElements(GL_TRIANGLES, task->n_index, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

inline void ideDraw(const DrawTask *const task, IDE * ide) {
  const float * const viewport = (const float *) &ide->window->info.viewport;
  switch (task->task_type) {
    case TT_LINES: {
      return ideDrawLines(task, viewport);
    }
    case TT_POLYLINE: {
      return ideDrawPolyline(task, viewport);
    }
    case TT_SOLID_AREA:
    case TT_TRIANGULATED_AREA: {
      return ideDrawArea(task, viewport);
    }
    case TT_TEXT: {
      return ideDrawText(ide, task, viewport);
    }
  }
}
