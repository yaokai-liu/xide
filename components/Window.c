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
 * Filename: Window.c
 * Creator: Yaokai Liu
 * Create Date: 2024-7-6
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "Window.h"
#include "color.h"
#include "runtime.h"

inline void Window_resize(Window *window, int32_t width, int32_t height) {
  glfwWindowResize(window->handle, width, height);
}


void Topbar_update(Widget *_topbar) {
  _topbar->box[BE_LEFT] = 0;
  _topbar->box[BE_TOP] = 0;
  // topbar as wide as the window
  _topbar->box[BE_RIGHT] = Widget_width((Widget *)_topbar->parent);
  ideMakeWidgetBox(_topbar->runtimeContext, _topbar);
  Box_update(_topbar);
}

#define lenof(_array)  (sizeof(_array) / sizeof(_array[0]))
inline void Window_setTextTitle(Window *window, Text *text) {
    Box *topbar = window->SUPER.allocator->calloc(1, sizeof(Box));
    topbar->SUPER.type = WT_BAR;
    topbar->SUPER.status = WS_FOCUSED;
    topbar->SUPER.property = WP_RE_GEO_TO_CHILDREN | WP_CHILD_CHILDREN;
    topbar->SUPER.allocator = window->SUPER.allocator;
    topbar->SUPER.runtimeContext = window->SUPER.runtimeContext;
    topbar->SUPER.parent = (Widget *) window;
    topbar->SUPER.box[BE_LEFT] = 0;
    topbar->SUPER.box[BE_TOP] = 0;

    topbar->children = Array_new(sizeof(Widget *), WT_WIDGET, topbar->SUPER.allocator);
    Box_append(topbar, (Widget *) text);
    text->SUPER.runtimeContext = topbar->SUPER.runtimeContext;

    // topbar as wide as the window
    topbar->SUPER.box[BE_RIGHT] = Widget_width((Widget *)window);
    topbar->SUPER.box[BE_BOTTOM] = Widget_height((Widget *)text) + 8;
    topbar->SUPER.funcDraw = Box_draw;
    topbar->SUPER.funcRange = nullptr;
    topbar->SUPER.funcUpdate = Topbar_update;

    window->bars[BE_TOP] = (Widget *) topbar;
}

Window *ideMakeWindow(IDE *ide, GLFWwindow *handle, const char_t *title) {

  Window * const window = ide->allocator->calloc(1, sizeof(Window));
  window->SUPER.type = WT_WINDOW;
  window->SUPER.property = WP_BOX_AS_GEOMETRY | WP_RE_GEO_TO_CHILDREN;
  window->SUPER.status = WS_FOCUSED;
  window->SUPER.allocator = ide->allocator;
  window->SUPER.runtimeContext = ide;
  window->SUPER.parent = nullptr;
  window->SUPER.funcDraw = Window_draw;
  window->SUPER.funcRange = nullptr;
  window->SUPER.funcUpdate = Window_update;
  GLint viewport[4] = {};
  glGetIntegerv(GL_VIEWPORT, viewport);
  window->SUPER.box[BG_X] = viewport[BG_X];
  window->SUPER.box[BG_Y] = viewport[BG_Y];
  window->SUPER.box[BG_W] = viewport[BG_W];
  window->SUPER.box[BG_H] = viewport[BG_H];

  window->handle = handle;

  // set window title
  const Font IDE_DEFAULT_FONT = {.path = "fonts/msyh.ttc", .index = 0, .size = 12};

  Text *text = window->SUPER.allocator->calloc(1, sizeof(Text));
  text->SUPER.type = WT_TEXT;
  text->SUPER.status = WS_FOCUSED;
  text->SUPER.property = WP_BOX_ALWAYS_RE_ADJUST;
  text->SUPER.allocator = window->SUPER.allocator;
  text->SUPER.funcDraw = Text_draw;
  text->SUPER.box[BE_LEFT] = 4;
  text->SUPER.box[BE_TOP] = 4;
  text->text = title;
  text->font = IDE_DEFAULT_FONT;
  text->mode = TS_RIGHT | TS_BELOW | TS_HORIZONTAL;
  text->color = RGB_WHITE;
  ideMakeText(ide, text);
  Window_setTextTitle(window, text);

  return window;
}

void Window_destroy(Window *window) {
  glfwDestroyWindow(window->handle);
  window->SUPER.allocator->free(window);
}

void Window_update(Widget *_window) {
  Window *window = (Window *)_window;
  if (window->central) { Widget_update(window->central); }
  if (window->bars[BE_LEFT]) { Widget_update(window->bars[BE_LEFT]); }
  if (window->bars[BE_TOP]) { Widget_update(window->bars[BE_TOP]); }
  if (window->bars[BE_RIGHT]) { Widget_update(window->bars[BE_RIGHT]); }
  if (window->bars[BE_BOTTOM]) { Widget_update(window->bars[BE_BOTTOM]); }
}

void Window_draw(Widget *_window) {
  Window *window = (Window *)_window;
  if (window->central) { Widget_draw(window->central); }
  if (window->bars[BE_LEFT]) { Widget_draw(window->bars[BE_LEFT]); }
  if (window->bars[BE_TOP]) { Widget_draw(window->bars[BE_TOP]); }
  if (window->bars[BE_RIGHT]) { Widget_draw(window->bars[BE_RIGHT]); }
  if (window->bars[BE_BOTTOM]) { Widget_draw(window->bars[BE_BOTTOM]); }
}