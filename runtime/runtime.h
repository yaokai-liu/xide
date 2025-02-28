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
 * Filename: runtime.h
 * Creator: Yaokai Liu
 * Create Date: 2024-7-6
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_RUNTIME_H
#define XIDE_RUNTIME_H

#include "draw.h"
#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "widgets.h"
#include "xgl-object.h"

#define REFER(a)             a *

#define rt_error(fmt, ...)   fprintf(stderr, "[ERROR] " fmt ".\n", ##__VA_ARGS__)
#define rt_message(fmt, ...) fprintf(stdout, "[INFO] " fmt ".\n", ##__VA_ARGS__)
#define rt_warning(fmt, ...) fprintf(stdout, "[WARNING] " fmt ".\n", ##__VA_ARGS__)
#define rt_debug(fmt, ...)   fprintf(stdout, "[DEBUG] " fmt ".\n", ##__VA_ARGS__)

void APIENTRY xglDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                             const GLchar *message, const void *userParam);

typedef struct {
  char_t *path;
  GLenum type;
} ShaderInfo;
GLuint *ideCompileShaders(IDE *ide, ShaderInfo shaderInfo[], uint32_t count);
int initializeGlad();
GLFWmonitor *switchMonitor(int index);
void switchWindow(IdeWindow *window);

void ideSetWindowTitle(IdeWindow *handle, const char_t *title);
void ideSetWindowSize(GLFWwindow *handle, int width, int height);
void ideWindowRefreshCallback(GLFWwindow *handle);
void ideProcessInput(IdeWindow *window);
IdeWindow *ideCreateWindow(int width, int height, const char_t *title, const Allocator *allocator);
void ideDestroyWindow(IdeWindow *window);

void ideDrawUiOnce(IDE *ide);
void ideAddTasks(IDE *ide, DrawTask *task, GLuint *shaderProgram);

bool ideShouldStopRender(IdeWindow *window);
void ideWindowShow(IDE *ide);

#endif  // XIDE_RUNTIME_H
