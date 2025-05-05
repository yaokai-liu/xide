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
#include "GLFW/glfw3.h"
#include "xgl-object.h"
#include "runtime-msg.h"

typedef struct {
  char_t *path;
  GLenum type;
} ShaderInfo;

GLuint *ideCompileShaders(IDE *ide, ShaderInfo shaderInfo[], uint32_t count);

int ideInitializeGlad();
GLFWwindow *ideInitGlfwGLContext(int width, int height);

GLFWmonitor *ideSwitchMonitor(int index);
void ideSwitchWindow(Window *window);

void ideDrawUiOnce(IDE *ide);
void ideAddTasks(IDE *ide, DrawTask *task, GLuint *shaderProgram);

void ideSetupUi(IDE *ide);
void ideShowWindow(IDE *ide);
bool ideShouldStopRender(Window *window);
void ideUpdateHoveredWidget(IDE *ide, uint32_t position[2]);
void ideUpdateMouseMovement(IDE *ide, uint32_t position[2]);
void idePassMouseLeftButtonEvent(IDE *ide, uint32_t event, uint32_t mods);
void ideShiftWindow(IDE *ide, const uint32_t vector[2]);
void ideResizeWindow(IDE *ide, GLint viewport[4]);

#endif  // XIDE_RUNTIME_H
