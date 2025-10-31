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
 * Module Name: widgets
 * Filename: Window.c
 * Creator: Yaokai Liu
 * Create Date: 2024-7-6
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "Window.h"
#include "runtime.h"

Window *Window_new(IDE *ide, GLFWwindow *handle) {
  Window * const window = ide->allocator->calloc(1, sizeof(Window));
  window->SUPER.type = WIDGET_TYPE_WINDOW;
  window->SUPER.property = WIDGET_PROPERTY_BOX_AS_GEOMETRY
                         | WIDGET_PROPERTY_RE_GEO_TO_CHILDREN;
  window->SUPER.status = WIDGET_STATUS_FOCUSED;
  window->SUPER.allocator = ide->allocator;
  window->SUPER.runtime = ide;
  window->SUPER.parent = nullptr;
  window->SUPER.graphic = nullptr;
  window->SUPER.funcDraw = Window_draw;
  window->SUPER.funcUpdateGraph = Window_updateGraph;
  window->SUPER.curSubWidget = Window_curSubWidget;
  // events of window will be immediately processed by the ide.
  window->SUPER.funcEventProc = Window_eventProcess;
  GLint viewport[4] = {};
  glGetIntegerv(GL_VIEWPORT, viewport);
  window->SUPER.box[BG_X] = viewport[BG_X];
  window->SUPER.box[BG_Y] = viewport[BG_Y];
  window->SUPER.box[BG_W] = viewport[BG_W];
  window->SUPER.box[BG_H] = viewport[BG_H];

  window->handle = handle;
  glfwGetWindowPos(handle,
                   (int *) &window->geometry[BG_X],
                   (int *) &window->geometry[BG_Y]);
  glfwGetWindowSize(handle,
                    (int *) &window->geometry[BG_W],
                    (int *) &window->geometry[BG_H]);
  glfwSetWindowUserPointer(window->handle, window);

  return window;
}

#define bar(b) (window->bars[b])
#define testLocal(w, c) ((w) && IdeWidget_testLocal(w, c))
void Window_destroy(Window *window) {
  glfwDestroyWindow(window->handle);
  window->SUPER.allocator->free(window);
}

void Window_updateGraph(Widget *_window) {
  Window *window = (Window *)_window;
  if (bar(BE_L)) { Widget_updateGraphic(bar(BE_L)); }
  if (bar(BE_T)) { Widget_updateGraphic(bar(BE_T)); }
  if (bar(BE_R)) { Widget_updateGraphic(bar(BE_R)); }
  if (bar(BE_B)) { Widget_updateGraphic(bar(BE_B)); }
  if (window->central) { Widget_updateGraphic(window->central); }
  ideMakeWindow(_window);
}

void Window_draw(const Widget *_window) {
  const Window *window = (const Window *)_window;
  if (_window->drawTask) { ideDraw(_window->runtime, _window->drawTask); }
  if (window->central) { Widget_draw(window->central); }
  if (bar(BE_L)) { Widget_draw(bar(BE_L)); }
  if (bar(BE_T)) { Widget_draw(bar(BE_T)); }
  if (bar(BE_R)) { Widget_draw(bar(BE_R)); }
  if (bar(BE_B)) { Widget_draw(bar(BE_B)); }
}

void Window_resize(Widget *_window, const uint32_t viewport[4]) {
  _window->box[BG_X] = viewport[BG_X];
  _window->box[BG_Y] = viewport[BG_Y];
  _window->box[BG_W] = viewport[BG_W];
  _window->box[BG_H] = viewport[BG_H];
  Window *window = (Window *)_window;
  if (bar(BE_T)) { Widget_reGeometry(bar(BE_T), _window->box); }
  if (window->central) { Widget_reGeometry(window->central, _window->box); }
}

Widget *Window_curSubWidget(const Widget *_window, uint32_t local_coord[2]) {
  Window *window = (Window *)_window;
  if (testLocal(bar(BE_L), local_coord)) { return bar(BE_L); }
  if (testLocal(bar(BE_T), local_coord)) { return bar(BE_T); }
  if (testLocal(bar(BE_R), local_coord)) { return bar(BE_R); }
  if (testLocal(bar(BE_B), local_coord)) { return bar(BE_B); }
  if (testLocal(window->central, local_coord)) { return window->central; }
  return nullptr;
}

void *Window_eventProcess(Widget *_window, uint32_t event_id, void *args) {
  return IdeWidget_eventProcess(_window, event_id, args);
}

void Window_measureBox(Window *window, uint32_t box[4], uint32_t box_edge) {
  Widget *_window = (Widget *) window;
  if (box_edge == BE_T || box_edge == BE_B) {
    box[BE_L] = window->bars[BE_L] && Widget_getBit(window->property, WINDOW_PROPERTY_LEFT_FULL)
                  ? Widget_width(window->bars[BE_L]) : 0;
    box[BE_R] = window->bars[BE_R] && Widget_getBit(window->property, WINDOW_PROPERTY_RIGHT_FULL)
                  ? (Widget_width(_window) - Widget_width(window->bars[BE_R])) : Widget_width(_window);
  }
  else if (box_edge == BE_L || box_edge == BE_R) {
    box[BE_T] = window->bars[BE_T] && !Widget_getBit(window->property, WINDOW_PROPERTY_LEFT_FULL)
                  ? Widget_height(window->bars[BE_T]) : 0;
    box[BE_B] = window->bars[BE_B] && !Widget_getBit(window->property, WINDOW_PROPERTY_RIGHT_FULL)
                  ? (Widget_height(_window) - Widget_height(window->bars[BE_B])) : Widget_height(_window);
  }
}

void Window_measureCentral(Window *window, uint32_t box[4]) {
  Widget *_window = (Widget *) window;
  box[BE_L] = window->bars[BE_L] ? Widget_width(window->bars[BE_L]) : 0;
  box[BE_R] = window->bars[BE_R] ? (Widget_width(_window) - Widget_width(window->bars[BE_R])) : Widget_width(_window);
  box[BE_T] = window->bars[BE_T] ? Widget_height(window->bars[BE_T]) : 0;
  box[BE_B] = window->bars[BE_B] ? (Widget_height(_window) - Widget_height(window->bars[BE_B])) : Widget_height(_window);
}

void ideMakeWindow(const Widget *) {}
