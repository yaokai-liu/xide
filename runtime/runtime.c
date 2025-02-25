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
 * Filename: runtime.c
 * Creator: Yaokai Liu
 * Create Date: 2024-11-15
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "runtime.h"
#include "char_t.h"
#include "shader.h"
#include <minmax.h>
#include <pthread.h>
#include <stdio.h>

GLuint *ideCompileShaders(IdeWindow *window, ShaderInfo shaderInfo[], uint32_t count) {
  int status;
  // shader program
  GLuint shaderProgram = glCreateProgram();
  for (int i = 0; i < min(4, count); i++) {
    const char_t *path = shaderInfo[i].path;
    const GLenum type = shaderInfo[i].type;
    if (path && type) {
      GLuint shader = compileShader(path, type, window->allocator);
      glAttachShader(shaderProgram, shader);
      glDeleteShader(shader);
    } else if (!path) {
      rt_message("not given shaders file path for type %d, skip", type);
    } else {
      rt_message("not given type %d of shaders, skip", type);
    }
  }
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
  if (!status) {
    GLchar infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    rt_error("Failed to link shaders program: \n%s", infoLog);
    glfwTerminate();
    return nullptr;
  }
  Array_append(window->shaderProgramArray, &shaderProgram, 1);
  return Array_last_virt(window->shaderProgramArray);
}

int initializeGlad() {
  int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  if (!status) {
    rt_error("failed to initialize GLAD%s", "");
    return -1;
  }
  rt_message("OpenGL Vendor: %s", glGetString(GL_VENDOR));
  rt_message("Using OpenGL Version: %s", glGetString(GL_VERSION));
  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags) {
    rt_message("%s", "Debug Enabled");
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(xglDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
  }
  return 0;
}

void switchWindow(IdeWindow *window) {
  glfwMakeContextCurrent(window->info.handle);
  glfwSwapInterval(1);
}

GLFWmonitor *switchMonitor(int index) {
  int monitorCount;
  GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);
  rt_message("found monitors: %d", monitorCount);
  for (int i = 0; i < monitorCount; i++) {
    const GLFWvidmode *mode = glfwGetVideoMode(monitors[i]);
    rt_message("size of monitor[%d]: %dx%d", i, mode->width, mode->height);
  }
  GLFWmonitor *monitor = monitors[index];
  const GLFWvidmode *mode = glfwGetVideoMode(monitors[index]);
  rt_message("switch to monitor[%d]: %dx%d", index, mode->width, mode->height);
  return monitor;
}

void ideWindowAddTasks(IdeWindow *window, DrawTask *task, GLuint *shaderProgram) {
  shaderProgram = Array_vert2real(window->shaderProgramArray, shaderProgram);
  xglBindShaderProgram(task, *shaderProgram);
  Array_append(window->drawTaskArray, task, 1);
}

void ideDrawUiOnce(IdeWindow *window) {
  if (window->central) { glClearColor(0.2f, 0.3f, 0.3f, 1.0f); }
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  const uint32_t n_tasks = Array_length(window->drawTaskArray);
  const DrawTask *tasks = Array_real_addr(window->drawTaskArray, 0);
  for (uint32_t i = 0; i < n_tasks; i++) { xglDraw(&tasks[i], window); }
  glfwSwapBuffers(window->info.handle);
}

inline void ideSetWindowTitle(IdeWindow *handle, const char_t *title) {
  handle->info.title = title;
}

IdeWindow *ideCreateWindow(const int width, const int height, const char_t *title,
                           const Allocator *allocator) {
  // TODO: loadPluginsFrom(directory) async;
  // TODO: loadProjectFrom(directory) async;
  // TODO: setupUiFrom(filepath) main thread;

  GLFWwindow *handle = glfwCreateWindow(width, height, title, NULL, NULL);
  // make context
  glfwMakeContextCurrent(handle);
  // set swap interval
  glfwSwapInterval(1);
  // initialize glad
  if (initializeGlad()) { return nullptr; }
  // set opengl viewport
  glViewport(0, 0, width, height);

  glfwSetWindowSizeCallback(handle, ideSetWindowSize);
  glfwSetWindowRefreshCallback(handle, ideWindowRefreshCallback);

  IdeWindow * const window = allocator->calloc(1, sizeof(IdeWindow));
  window->allocator = allocator;
  glfwSetWindowUserPointer(handle, window);

  window->info.handle = handle;

  int pos_x, pos_y;
  glfwGetWindowPos(handle, &pos_x, &pos_y);
  window->info.geometry[0] = pos_x;
  window->info.geometry[0] = pos_y;
  window->info.geometry[0] = width;
  window->info.geometry[0] = height;

  window->info.title = title;

  GLint viewport[4] = {0, 0, width, height};
  window->info.viewport[0] = (float) viewport[0];
  window->info.viewport[1] = (float) viewport[1];
  window->info.viewport[2] = (float) viewport[2];
  window->info.viewport[3] = (float) viewport[3];

  window->drawTaskArray = Array_new(sizeof(DrawTask), enum_XGL_DRAW_TASK, allocator);
  window->shaderProgramArray = Array_new(sizeof(GLuint), enum_XGL_SHADER_PROG, allocator);
  window->shaderArray = Array_new(sizeof(GLuint), enum_XGL_SHADER, allocator);

  return window;
}

void ideDestroyWindow(IdeWindow *window) {
  Array_reset(window->drawTaskArray, (destruct_t *) xglDestroyDrawTask);
  Array_destroy(window->drawTaskArray);
  releasePrimeArray(window->shaderProgramArray);
  glfwDestroyWindow(window->info.handle);
  window->allocator->free(window);
}

void *ideRepeatDrawUi(IdeWindow *window) {
  while (!ideShouldStopRender(window)) {
    ideProcessInput(window);
    ideDrawUiOnce(window);
    glfwPollEvents();
  }
  return nullptr;
}

bool ideShouldStopRender(IdeWindow *window) {
  return glfwWindowShouldClose(window->info.handle);
}

void ideShow(IdeWindow *window) {
  ideDrawUiOnce(window);
  pthread_t uiThread;
  pthread_create(&uiThread, nullptr, (void *(*) (void *) ) ideRepeatDrawUi, window);
  pthread_detach(uiThread);
  while (!ideShouldStopRender(window)) { glfwPollEvents(); }
  void *res;
  pthread_join(uiThread, &res);
}