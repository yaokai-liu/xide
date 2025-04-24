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
 * Module Name: components
 * Filename: window.c
 * Creator: Yaokai Liu
 * Create Date: 2024-7-6
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "Window.h"
#include "runtime.h"

inline void Window_resize(Window *window, int32_t width, int32_t height) {
  glfwWindowResize(window->handle, width, height);
}

inline void Window_setTextTitle(Window *window, Text *text) {
    Widget *topbar = window->SUPER.allocator->calloc(1, sizeof(Widget));
    topbar->type = WT_BAR;
    topbar->status = WS_FOCUSED;
    topbar->property = WP_RE_GEO_TO_CHILDREN | WP_CHILD_CHILDREN | WP_BOX_AS_GEOMETRY;
    topbar->allocator = window->SUPER.allocator;
    topbar->parent = (Widget *) window;
    topbar->box[BE_TOP] = 0;
    topbar->box[BE_LEFT] = 0;
    // topbar as wide as the window
    topbar->box[BE_RIGHT] = Widget_width((Widget *)window);
    topbar->box[BE_BOTTOM] = Widget_height((Widget *)text) + 10;
    topbar->funcRange = nullptr;
    topbar->funcColor = nullptr;
    topbar->funcUpdate = nullptr;
    topbar->child.children = Array_new(sizeof(Widget *), WT_WIDGET, topbar->allocator);
    IdeWidget_append(topbar, (Widget *) text);
    window->bars[BE_TOP] = topbar;


}

Window *ideMakeWindow(IDE *ide, int width, int height, const char_t *title) {
  rt_message("Using GLFW Version: %d.%d", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR);
  // Required OpenGL version: 4.5.0
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_DECORATED, GLFW_WIN_DECO_NO_TITLE_BAR);

  // TODO: loadPluginsFrom(directory) async;
  // TODO: loadProjectFrom(directory) async;
  // TODO: setupUiFrom(filepath) main thread;

  if (!title) { title = "xIDE"; }

  GLFWwindow *handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
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
  if (initializeGlad()) { return nullptr; }
  // set opengl viewport
  glViewport(0, 0, width, height);

  glfwSetWindowSizeCallback(handle, glfwWindowResize);
  glfwSetWindowRefreshCallback(handle, glfwWindowRefresh);

  Window * const window = ide->allocator->calloc(1, sizeof(Window));
  window->SUPER.type = WT_WINDOW;
  window->SUPER.property = WP_BOX_AS_GEOMETRY | WP_RE_GEO_TO_CHILDREN;
  window->SUPER.status = WS_FOCUSED;
  window->SUPER.allocator = ide->allocator;
  window->SUPER.parent = nullptr;
  window->SUPER.funcRange = nullptr;
  window->SUPER.funcColor = nullptr;
  window->SUPER.funcUpdate = nullptr;

  int pos_x, pos_y;
  glfwGetWindowPos(handle, &pos_x, &pos_y);
  window->SUPER.box[BE_LEFT] = pos_x;
  window->SUPER.box[BE_TOP] = pos_y;
  window->SUPER.box[BE_RIGHT] = pos_x + width;
  window->SUPER.box[BE_BOTTOM] = pos_y + height;

  GLint viewport[4] = {0, 0, width, height};
  window->viewport[BG_X] = (float) viewport[0];
  window->viewport[BG_Y] = (float) viewport[1];
  window->viewport[BG_W] = (float) viewport[2];
  window->viewport[BG_H] = (float) viewport[3];

  window->handle = handle;
  glfwSetWindowUserPointer(window->handle, ide);

  // set window title
  const Font IDE_DEFAULT_FONT = {.path = "fonts/JetBrainsMono-Regular.ttf", .index = 0, .size = 14};

  Text *text = window->SUPER.allocator->calloc(1, sizeof(Text));
  text->SUPER.type = WT_TEXT;
  text->SUPER.status = WS_FOCUSED;
  text->SUPER.property = WP_BOX_ALWAYS_RE_ADJUST;
  text->SUPER.allocator = window->SUPER.allocator;
  text->SUPER.box[BE_LEFT] = 5;
  text->SUPER.box[BE_TOP] = 5;
  text->text = title;
  text->font = IDE_DEFAULT_FONT;
  text->mode = TS_RIGHT | TS_BELOW | TS_HORIZONTAL;
  ideMakeText(ide, text);
  Window_setTextTitle(window, text);

  return window;
}

void Window_destroy(Window *window) {
  glfwDestroyWindow(window->handle);
  window->SUPER.allocator->free(window);
}
