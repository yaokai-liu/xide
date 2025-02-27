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

#include "enum.h"
#include "ide.h"
#include "print.h"
#include "runtime.h"
#include "shader.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  const Allocator * const allocator = &STDAllocator;

  char_t workdir[512] = {};
  {
    int length = 0;
    for (int i = 0; argv[0][i]; i++) {
      if (argv[0][i] == '\\' || argv[0][i] == '/') { length = i; }
    }
    allocator->memcpy(workdir, argv[0], length);
    workdir[length] = '\0';
  }

  if (!glfwInit()) { return -1; }
  rt_message("Using GLFW Version: %d.%d", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR);
  // Required OpenGL version: 4.6.0
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

  IdeWindow *mainWindow = ideCreateWindow(1000, 800, "xIDE - {.projectName}", allocator);
  if (!mainWindow) {
    glfwTerminate();
    return -1;
  }
  mainWindow->workdir = workdir;
  ShaderInfo shaderInfos[] = {
    {"shaders/vert-default.glsl", GL_VERTEX_SHADER  },
    {"shaders/frag-default.glsl", GL_FRAGMENT_SHADER}
  };
  GLuint *shader = ideCompileShaders(mainWindow, shaderInfos, 2);

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
  };
  Array *vertex_array = Array_new(sizeof(Vertex2D), enum_XGL_COORD, allocator);
  Array_append(vertex_array, vertices, 10);
  task = xglCreatePolygon2D(vertex_array, 0, false, allocator);
  ideWindowAddTasks(mainWindow, task, shader);
  //  allocator->free(task);
  //  Array_reset(vertex_array, nullptr);

  //  for (int i = 0; i < 100; i ++) {
  //    Vertex2D vert = {
  //        .coord = {
  //          400 + 200 * cosf(2 * (float) M_PI / 100 * (float) i),
  //          400 + 200 * sinf(2 * (float) M_PI / 100 * (float) i)
  //        },
  //        .color = 0xFFFF00FF
  //    };
  //    Array_append(vertex_array, &vert, 1);
  //  }
  //  Vertex2D center = { .coord = {400.0f, 400.0f }, .color = 0xFFFF00FF};
  //  Array_append(vertex_array, &center, 1);
  //  task = xglCreateCurveArea2D(vertex_array, 0, true, true, allocator);
  //  ideWindowAddTasks(mainWindow, task, 0);
  //  allocator->free(task);
  //  releasePrimeArray(vertex_array);

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
  //  Array *line_array = Array_new(sizeof(Line), enum_XGL_LINE, allocator);
  //  Array_append(line_array, lines, 4);
  //  task = xglCreatePixelLines(line_array, 0, allocator);
  //  ideWindowAddTasks(mainWindow, task, 0);
  //  allocator->free(task);
  //  releasePrimeArray(line_array);

  IDE *ide = IDE_new(allocator);
//  Font font = {.path = "SourceHanSerifSC-Regular.otf", .index = 0, .size = 64};
  Font font = {.path = "JetBrainsMono-Regular.ttf", .index = 0, .size = 16};
  ShaderInfo shaderInfos2[] = {
    {"shaders/char-vert.glsl", GL_VERTEX_SHADER  },
    {"shaders/char-frag.glsl", GL_FRAGMENT_SHADER}
  };
  shader = ideCompileShaders(mainWindow, shaderInfos2, 2);

  Array *char_array = Array_new(sizeof(char_t), enum_IDE_CHAR, allocator);
  #define text "Hello OpenGL and FreeType"
  Array_append(char_array, text, sizeof(text) - 1);
//  task = ideCreatePrint2D(ide, char_array, vertex_array, 0, &font);
  task = ideCreateText2D(ide, char_array, &vertices[3], -2,
                         TS_RIGHT | TS_HORIZONTAL, 0, &font);
  ideWindowAddTasks(mainWindow, task, shader);

  glLineWidth(1);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  ideShow(mainWindow);

  ideDestroyWindow(mainWindow);
  glfwTerminate();
  return 0;
}
