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
#include "minmax.h"
#include <pthread.h>
#include <stdio.h>
#include "callback.h"
#ifndef PATH_MAX
  #define PATH_MAX 256
#endif

enum PATH_TYPE {
  PT_RELATED = 0,
  PT_UNIX_ABS = 1,
  PT_WIN_ABS = 2,
};

uint32_t isAbsolutePath(const char_t *path) {
  if (path[0] == '/') { return PT_UNIX_ABS; }
  if ('A' <= path[0] && path[0] <= 'Z' && path[1] == ':' && path[2] == '\\') { return PT_WIN_ABS; }
  return PT_RELATED;
}

char_t *ideResolveToAbsolutePath(IDE *ide, char_t *path, char_t *dest) {
  if (isAbsolutePath(path) != PT_RELATED) { return path; }
  uint32_t dir_len = strlen(ide->workdir);
  uint32_t path_len = dir_len + strlen(path);
  if (path_len - 2 > PATH_MAX) {
    rt_message("path is too long to resolve: %s/%s", ide->workdir, path);
    return nullptr;
  }
  sprintf(dest, "%s/%s", ide->workdir, path);
  return dest;
}

GLuint compileShader(const char_t *path, const GLenum type, const Allocator *allocator) {
  FILE *file = fopen(path, "r");
  if (!file) {
    rt_error("Failed to open shader file: '%s'", path);
    return 0;
  }
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);
  GLchar *source = allocator->malloc((sizeof(char) * length) + 1);
  fread((void *) source, sizeof(char), length, file);
  source[length] = 0;
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, (const GLchar **) &source, NULL);
  glCompileShader(shader);
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    rt_error("Failed to compile shader '%s': \n%s\n", path, infoLog);
  }
  return shader;
}

GLuint *ideCompileShaders(IDE *ide, ShaderInfo shaderInfo[], uint32_t count) {
  int status;
  // shader program
  char_t resolved_path[PATH_MAX] = {};
  GLuint shaderProgram = glCreateProgram();
  for (int i = 0; i < min(4, count); i++) {
    char_t *path = ideResolveToAbsolutePath(ide, shaderInfo[i].path, resolved_path);
    const GLenum type = shaderInfo[i].type;
    if (path && type) {
      GLuint shader = compileShader(path, type, ide->allocator);
      glAttachShader(shaderProgram, shader);
      glDeleteShader(shader);
    } else if (!path) {
      rt_message("not given shaders file path for type %d, skip", type);
      return nullptr;
    } else {
      rt_message("not given type %d of shaders, skip", type);
      return nullptr;
    }
  }
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
  if (!status) {
    GLchar infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    rt_error("Failed to link shaders program: \n%s", infoLog);
    return nullptr;
  }
  Array_append(ide->shaderProgramArray, &shaderProgram, 1);
  return Array_last_virt(ide->shaderProgramArray);
}

int ideInitializeGlad() {
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
    glDebugMessageCallback(xglCallback_debugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
  }
  return 0;
}

void ideSwitchWindow(Window *window) {
  glfwMakeContextCurrent(window->handle);
  glfwSwapInterval(1);
}

GLFWmonitor *ideSwitchMonitor(int index) {
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

void ideAddTasks(IDE *ide, DrawTask *task, GLuint *shaderProgram) {
  if (!task || !shaderProgram) { return; }
  shaderProgram = Array_virt2real(ide->shaderProgramArray, shaderProgram);
  xglBindShaderProgram(task, *shaderProgram);
  const DrawTask *tasks = Array_real_addr(ide->drawTaskArray, 0);
  uint32_t n_tasks = Array_length(ide->drawTaskArray);
  uint32_t ndx = n_tasks / 2;
  while (n_tasks) {
    const uint32_t depth = tasks[ndx].depth;
    if (task->depth == depth) { break; }
    n_tasks /= 2;
    ndx += task->depth < depth ? -n_tasks : n_tasks;
  }
  Array_insert(ide->drawTaskArray, ndx, task, 1);
}

void ideDrawUiOnce(IDE *ide) {
//  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  Widget_draw((Widget *) ide->mainWindow);
  glfwSwapBuffers(ide->mainWindow->handle);
}

void *ideRepeatDrawUi(IDE *ide) {
  while (!ideShouldStopRender(ide->mainWindow)) {
    ideWindowProcessInput(ide->mainWindow);
    ideDrawUiOnce(ide);
    glfwWaitEvents();
  }
  return nullptr;
}

bool ideShouldStopRender(Window *window) {
  return glfwWindowShouldClose(window->handle);
}

void ideWindowShow(IDE *ide) {
  ideRepeatDrawUi(ide);
  //  pthread_t uiThread;
  //  pthread_create(&uiThread, nullptr, (void *(*) (void *) ) ideRepeatDrawUi, ide);
  //  pthread_detach(uiThread);
  //  while (!ideShouldStopRender(ide->window)) { glfwPollEvents(); }
  //  void *res;
  //  pthread_join(uiThread, &res);
}

GLFWwindow *ideInitGlfwGLContext(int width, int height) {
  rt_message("Using GLFW Version: %d.%d, build from source code", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR);
  // Required OpenGL version: 4.6.0
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 0);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_DECORATED, GLFW_WIN_DECO_NO_TITLE_BAR);

  // TODO: loadPluginsFrom(directory) async;
  // TODO: loadProjectFrom(directory) async;
  // TODO: setupUiFrom(filepath) main thread;

  GLFWwindow *handle = glfwCreateWindow(width, height, "", nullptr, nullptr);
  if (!handle) {
    const char_t *err_msg = nullptr;
    glfwGetError(&err_msg);
    rt_error("failed to create GLFW window: %s", err_msg);
    return nullptr;
  }
  // make context
  glfwMakeContextCurrent(handle);
  // set swap interval
  glfwSwapInterval(1);
  // initialize glad
  if (ideInitializeGlad()) { return nullptr; }
  // set opengl viewport
  glViewport(0, 0, width, height);

  glfwSetCursorPosCallback(handle, ideCallback_cursorPosition);
  glfwSetWindowSizeCallback(handle, ideCallback_windowResize);
  glfwSetWindowRefreshCallback(handle, ideCallback_windowRefresh);
  return handle;
}

void ideUpdateHoveredWidgetStack(IDE *ide, uint32_t position[2]) {
  Widget *widget = nullptr;
  while (!widget) { IDE_popHovered(ide, &widget); }
  IdeWidget_global2local(widget, position);
  while (!IdeWidget_testLocal(widget, position)) {
    rt_debug("cursor leaves %p", widget);
    IDE_popHovered(ide, &widget);
    if (widget->funcEventProc) { widget->funcEventProc(widget, enum_EVENT_CURSOR_LEAVE, nullptr); }
  }
  while (widget && IdeWidget_testLocal(widget, position)) {
    rt_debug("cursor enters %p", widget);
    IDE_pushHovered(ide, &widget);
    if (widget->funcEventProc) { widget->funcEventProc(widget, enum_EVENT_CURSOR_ENTER, nullptr); }
    widget = widget->getSubWidget ? widget->getSubWidget(widget, position) : nullptr;
  }
}
