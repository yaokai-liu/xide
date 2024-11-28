/**
 * Project Name: xide
 * Module Name: runtime
 * Filename: runtime.c
 * Creator: Yaokai Liu
 * Create Date: 2024-11-15
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "runtime.h"
#include <stdio.h>

GLFWmonitor *switchMonitor(int index, int *width, int *height) {
  int monitorCount;
  GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);
  rt_message("found monitors: %d", monitorCount);
  for (int i = 0; i < monitorCount; i++) {
    const GLFWvidmode *mode = glfwGetVideoMode(monitors[i]);
    rt_message("size of monitor[i]: %dx%d", mode->width, mode->height);
  }
  GLFWmonitor *monitor = monitors[index];
  const GLFWvidmode *mode = glfwGetVideoMode(monitors[index]);
  *width = mode->width;
  *height = mode->height;
  rt_message("switch to monitor[i]: %dx%d", mode->width, mode->height);
  return monitor;
}

void ideWindowAddTasks(IdeWindow *window, DrawTask *task, int count) {
  Array_append(window->drawTaskList, task, count);
}

void ideDrawUI(IdeWindow *window) {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  const int n_tasks = (int) Array_length(window->drawTaskList);
  const DrawTask *tasks = Array_get(window->drawTaskList, 0);
  for (int i = 0; i < n_tasks; i++) { xglDraw(&tasks[i], window); }
  glfwSwapBuffers(window->info.handle);
}

IdeWindow *ideCreateWindow(GLFWwindow *handle, const Allocator *allocator) {
  // TODO: loadPluginsFrom(directory) async;
  // TODO: loadProjectFrom(directory) async;
  // TODO: setupUiFrom(filepath) main thread;
  IdeWindow *window = allocator->calloc(1, sizeof(IdeWindow));
  window->info.handle = handle;
  int pos_x, pos_y, width, height;
  glfwGetWindowPos(handle, &pos_x, &pos_y);
  glfwGetWindowSize(handle, &width, &height);
  glfwSetWindowUserPointer(handle, window);
  window->info.posX = pos_x;
  window->info.posY = pos_y;
  window->info.width = width;
  window->info.height = height;

  GLint viewport[4] = {};
  glGetIntegerv(GL_VIEWPORT, viewport);
  window->viewport[0] = (float) viewport[0];
  window->viewport[1] = (float) viewport[1];
  window->viewport[2] = (float) viewport[2];
  window->viewport[3] = (float) viewport[3];

  window->drawTaskList = Array_new(sizeof(DrawTask), allocator);
  window->allocator = allocator;
  return window;
}

void ideDestroyWindow(IdeWindow *window) {
  const int n_tasks = (int) Array_length(window->drawTaskList);
  DrawTask *tasks = Array_get(window->drawTaskList, 0);
  for (int i = 0; i < n_tasks; i++) { xglDestroyDrawTask(&tasks[i]); }
  Array_reset(window->drawTaskList, nullptr);
  Array_destroy(window->drawTaskList);
  glfwDestroyWindow(window->info.handle);
  window->allocator->free(window);
}
