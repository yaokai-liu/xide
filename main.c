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
 * Module Name:
 * Filename: main.c
 * Creator: Yaokai Liu
 * Create Date: 2025-2-22
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "ide.h"
#include "print.h"
#include "runtime.h"
#include "enum.h"
#include <stdio.h>
#if defined(_WIN32) || defined(_WIN64)
  #include <direct.h>
#else
#include <unistd.h>
#endif
#include <math.h>

int main(int argc, char *argv[]) {
  const Allocator * const allocator = &STDAllocator;

  char_t workdir[PATH_MAX] = {};
  getcwd(workdir, PATH_MAX);

  if (!glfwInit()) { return -1; }
  IDE *ide = IDE_new(workdir, allocator);
  if (!ide) { glfwTerminate(); return -1; }

  ShaderInfo shaderInfos[] = {
    {"shaders/vert-default.glsl", GL_VERTEX_SHADER  },
    {"shaders/frag-default.glsl", GL_FRAGMENT_SHADER}
  };
  GLuint *shader = ideCompileShaders(ide, shaderInfos, 2);
  if (!shader) {
    IDE_destroy(ide);
    glfwTerminate();
  }

  DrawTask *task;

  Vertex2D vertices[] = {
    {.coord = {200.0f, 400.0f}, .color = 0xFFFFFFFF},
    {.coord = {300.0f, 200.0f}, .color = 0xFF00FFFF},
    {.coord = {500.0f, 100.0f}, .color = 0xFFFF00FF},
    {.coord = {600.0f, 300.0f}, .color = 0x00FFFFFF},
    {.coord = {700.0f, 600.0f}, .color = 0xFF00FFFF},
    {.coord = {800.0f, 800.0f}, .color = 0xFFFF00FF},
    {.coord = {900.0f, 700.0f}, .color = 0x00FFFFFF},
    {.coord = {800.0f, 900.0f}, .color = 0xFFFF00FF},
    {.coord = {500.0f, 800.0f}, .color = 0xFFFF00FF},
    {.coord = {300.0f, 600.0f}, .color = 0xFFFF00FF},
    {.coord = {300.0f, 800.0f}, .color = 0xFFFF00FF},
//    {.coord = {350.0f, 750.0f}, .color = 0xFFFF00FF},
  };
  Array *vertex_array = Array_new(sizeof(Vertex2D), enum_XGL_COORD, allocator);
  Array_append(vertex_array, vertices, 11);
  task = ideCreatePolygon2D(vertex_array, 50, false, allocator);
  ideAddTasks(ide, task, shader);
  allocator->free(task);
  Array_reset(vertex_array, nullptr);

  for (int i = 0; i < 100; i ++) {
    Vertex2D vert = {
        .coord = {
          400 + 200 * cosf(2 * (float) M_PI / 100 * (float) i),
          400 + 200 * sinf(2 * (float) M_PI / 100 * (float) i)
        },
        .color = 0xFF2200FF
    };
    Array_append(vertex_array, &vert, 1);
  }
  Vertex2D center = { .coord = {400.0f, 400.0f }, .color = 0xFFFF77FF};
  Array_append(vertex_array, &center, 1);
  task = ideCreateCurveArea2D(vertex_array, 10, true, true, allocator);
  ideAddTasks(ide, task, shader);
  allocator->free(task);
  releasePrimeArray(vertex_array);

  Line lines[] = {
    {
     {100, 100, 0x00FF00FF},
     {700, 100, 0x00FF00FF},
     },
    {
     {700, 100, 0x00FF00FF},
     {700, 500, 0x00FF00FF},
     },
    {
     {700, 500, 0x00FF00FF},
     {100, 500, 0x00FF00FF},
     },
    {
     {100, 500, 0x00FF00FF},
     {100, 100, 0x00FF00FF},
     },
  };
  Array *line_array = Array_new(sizeof(Line), enum_XGL_LINE, allocator);
  Array_append(line_array, lines, 4);
  task = ideCreatePixelLines(line_array, 0, allocator);
  ideAddTasks(ide, task, shader);
  allocator->free(task);
  releasePrimeArray(line_array);

  Font font = {.path = "fonts/SourceHanSerifSC-Regular.otf", .index = 0, .size = 32};
  Font font1 = {.path = "fonts/SourceHanSerifSC-Regular.otf", .index = 0, .size = 16};
  Font font2 = {.path = "fonts/JetBrainsMono-Regular.ttf", .index = 0, .size = 16};
  ShaderInfo shaderInfos2[] = {
    {"shaders/char-vert.glsl", GL_VERTEX_SHADER  },
    {"shaders/char-frag.glsl", GL_FRAGMENT_SHADER}
  };
  shader = ideCompileShaders(ide, shaderInfos2, 2);
  if (!shader) {
    IDE_destroy(ide);
    glfwTerminate();
  }

  Array *char_array = Array_new(sizeof(char_t), enum_IDE_CHAR, allocator);
  char_t char_set[0x80] = {};
  for (uint32_t i = 'a'; i < 'z'; i ++) { char_set[i - 'a'] = (char_t) i; }
  Array_append(char_array, char_set, 'z' - 'a');
  vertex_array = Array_new(sizeof(Vertex2D), enum_XGL_COORD, allocator);
  Array_append(vertex_array, vertices, 11);
  task = ideCreateText2DByArray(ide, char_array, vertex_array, 50, &font);
  ideAddTasks(ide, task, shader);
  releasePrimeArray(vertex_array);
  releasePrimeArray(char_array);

  #define TEXT "Hello OpenGL and FreeType"
  XGLVector2D size = {};
  Vertex2D anchor = {.coord = {500.0f, 100.0f}, .color = 0xFFFF00FF};
  DrawTask *task1 = ideCreateTextStr2DByStr(ide, TEXT" 1", &anchor, -0.25f,
                                          TS_RIGHT | TS_ABOVE | TS_HORIZONTAL, 0, &font1, size);
  ideAddTasks(ide, task1, shader);
  anchor.coord[AXIS_Y] -= size[AXIS_Y] + 5;
  DrawTask *task2 = ideCreateTextStr2DByStr(ide, TEXT " 2", &anchor, -1.5f,
                                          TS_RIGHT | TS_ABOVE | TS_HORIZONTAL, 0, &font2, size);
  ideAddTasks(ide, task2, shader);

  glLineWidth(1);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  ideWindowShow(ide);

  IDE_destroy(ide);
  glfwTerminate();
  return 0;
}
