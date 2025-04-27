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
#include "minmax.h"
#include "runtime.h"
#include "widgets.h"

void Topbar_resize(Widget *_topbar);
void *Topbar_eventProcess(Widget *_topbar, uint32_t event_id, void *args);

#define lenof(_array)  (sizeof(_array) / sizeof(_array[0]))
inline void Window_setTextTitle(Window *window, const char_t *title) {
    Box *topbar = window->SUPER.allocator->calloc(1, sizeof(Box));
    topbar->SUPER.type = WT_BAR;
    topbar->SUPER.status = WS_FOCUSED;
    topbar->SUPER.property = WP_RE_GEO_TO_CHILDREN | WP_BOX_AS_GEOMETRY;
    topbar->SUPER.allocator = window->SUPER.allocator;
    topbar->SUPER.runtimeContext = window->SUPER.runtimeContext;
    topbar->SUPER.parent = (Widget *) window;
    topbar->SUPER.box[BG_X] = 0;
    topbar->SUPER.box[BG_Y] = 0;
    topbar->SUPER.box[BG_W] = Widget_width((Widget *)window);
    topbar->SUPER.box[BG_H] = 0;
    topbar->SUPER.funcDraw = Box_draw;
    topbar->SUPER.funcRange = nullptr;
    topbar->SUPER.getSubWidget = Box_getSubWidget;
    topbar->SUPER.funcEventProc = Topbar_eventProcess;
    topbar->SUPER.padding[BE_L] = 10;
    topbar->SUPER.padding[BE_R] = 10;
    topbar->SUPER.padding[BE_T] = 5 ;
    topbar->SUPER.padding[BE_B] = 5 ;
    topbar->children = Array_new(sizeof(Widget *), WT_WIDGET, topbar->SUPER.allocator);

    // set window title
    const Font IDE_DEFAULT_FONT = {.path = "fonts/msyh.ttc", .index = 0, .size = 12};

    Text *text = window->SUPER.allocator->calloc(1, sizeof(Text));
    text->SUPER.type = WT_TEXT;
    text->SUPER.status = WS_FOCUSED;
    text->SUPER.property = WP_BOX_ALWAYS_RE_ADJUST;
    text->SUPER.allocator = window->SUPER.allocator;
    text->SUPER.funcDraw = Text_draw;
    text->SUPER.funcRange = nullptr;
    text->SUPER.funcEventProc = Text_eventProcess;
    text->SUPER.box[BE_L] = topbar->SUPER.padding[BE_L];
    text->SUPER.box[BE_T] = topbar->SUPER.padding[BE_T] + 10;
    text->text = title;
    text->font = IDE_DEFAULT_FONT;
    text->mode = TS_RIGHT | TS_V_CENTER | TS_HORIZONTAL;
    text->color = RGB_WHITE;

    Box_append(topbar, (Widget *) text);

    window->bars[BE_T] = (Widget *) topbar;
}

Window *Window_new(IDE *ide, GLFWwindow *handle, const char_t *title) {
  Window * const window = ide->allocator->calloc(1, sizeof(Window));
  window->SUPER.type = WT_WINDOW;
  window->SUPER.property = WP_BOX_AS_GEOMETRY | WP_RE_GEO_TO_CHILDREN;
  window->SUPER.status = WS_FOCUSED;
  window->SUPER.allocator = ide->allocator;
  window->SUPER.runtimeContext = ide;
  window->SUPER.parent = nullptr;
  window->SUPER.funcRange = nullptr;
  window->SUPER.funcDraw = Window_draw;
  window->SUPER.getSubWidget = Window_getSubWidget;
  GLint viewport[4] = {};
  glGetIntegerv(GL_VIEWPORT, viewport);
  window->SUPER.box[BG_X] = viewport[BG_X];
  window->SUPER.box[BG_Y] = viewport[BG_Y];
  window->SUPER.box[BG_W] = viewport[BG_W];
  window->SUPER.box[BG_H] = viewport[BG_H];
  Window_setTextTitle(window, title);

  window->handle = handle;
  glfwSetWindowUserPointer(window->handle, window);

  return window;
}

#define bar(b) (window->bars[b])
#define testLocal(w, c) ((w) && IdeWidget_testLocal(w, c))
void Window_destroy(Window *window) {
  glfwDestroyWindow(window->handle);
  window->SUPER.allocator->free(window);
}

void Window_makeGraphic(Window *window) {
  if (window->central) { Widget_makeGraphic(window->central); }
  if (bar(BE_L)) { Widget_makeGraphic(bar(BE_L)); }
  if (bar(BE_T)) { Widget_makeGraphic(bar(BE_T)); }
  if (bar(BE_R)) { Widget_makeGraphic(bar(BE_R)); }
  if (bar(BE_B)) { Widget_makeGraphic(bar(BE_B)); }
}

void Window_draw(Widget *_window) {
  Window *window = (Window *)_window;
  if (window->central) { Widget_draw(window->central); }
  if (bar(BE_L)) { Widget_draw(bar(BE_L)); }
  if (bar(BE_T)) { Widget_draw(bar(BE_T)); }
  if (bar(BE_R)) { Widget_draw(bar(BE_R)); }
  if (bar(BE_B)) { Widget_draw(bar(BE_B)); }
}

#define barEventProcess(b, eid, args) do { \
if (bar(b) && bar(b)->funcEventProc) { bar(b)->funcEventProc(bar(b), eid, args); } \
} while (false)
void Window_resize(Widget *_window, const uint32_t viewport[4]) {
  _window->box[BG_X] = viewport[BG_X];
  _window->box[BG_Y] = viewport[BG_Y];
  _window->box[BG_W] = viewport[BG_W];
  _window->box[BG_H] = viewport[BG_H];
  Window *window = (Window *)_window;
  Topbar_resize(bar(BE_T));
}

Widget *Window_getSubWidget(Widget *_window, uint32_t local_coord[2]) {
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

void ideMakeWindow(IDE *ide, Widget *_window) {}

void Topbar_makeGraphic(Widget *_topbar) {
  Box *topbar = (Box *)_topbar;
  if (topbar->children) {
    uint32_t n_children = Array_length(topbar->children);
    Widget * const*children = Array_first_real(topbar->children);
    for (uint32_t i = 0; i < n_children; i++) {
      Widget_makeGraphic(children[i]);
    }
  }
  ideMakeBox(_topbar->runtimeContext, _topbar);
}

void Topbar_resize(Widget *_topbar) {
  Box *topbar = (Box *)_topbar;
  uint32_t new_width = Widget_width((Widget *)_topbar->parent);
  uint32_t new_height = min(_topbar->box[BG_H], Widget_height((Widget *)_topbar->parent));
  if (new_width == _topbar->box[BG_W] && new_height == _topbar->box[BG_H]) { return; }
  _topbar->box[BG_W] = new_width;
  _topbar->box[BG_H] = 0;
  // topbar as wide as the window
  uint32_t stoke = _topbar->padding[BE_T];
  if (topbar->children) {
    uint32_t n_children = Array_length(topbar->children);
    Widget * const*children = Array_first_real(topbar->children);
    for (uint32_t i = 0; i < n_children; i++) {
      uint32_t box[4] = {
        children[i]->box[0], children[i]->box[1],
        children[i]->box[2], children[i]->box[3],
      };
      Widget_reGeometry(children[i], _topbar->box);
      int c = memcmp(children[i]->box, box, sizeof(uint32_t) * 4);
      if (c != 0) { Widget_makeGraphic(children[i]); }
      stoke = max(stoke, Widget_bottom(children[i]));
    }
  }
  _topbar->box[BG_H] = stoke + _topbar->padding[BE_B];
  ideMakeBox(_topbar->runtimeContext, _topbar);
}

void *Topbar_eventProcess(Widget *_topbar, uint32_t event_id, void *args) {
  switch (event_id) {
    case enum_EVENT_CURSOR_MOVE: {
      uint32_t *position = args;
      if (Widget_getBit(_topbar->status, WS_PRESSED)) {
        uint32_t vector[2] = {
          [AXIS_X] = position[AXIS_X] - (uint32_t) (uint64_t) _topbar->msgData[AXIS_X],
          [AXIS_Y] = position[AXIS_Y] - (uint32_t) (uint64_t) _topbar->msgData[AXIS_Y],
        };
        ideShiftWindowPos(_topbar->runtimeContext, vector);
      } else {
        _topbar->msgData[AXIS_X] = (void *) (uint64_t) position[AXIS_X];
        _topbar->msgData[AXIS_Y] = (void *) (uint64_t) position[AXIS_Y];
      }
      break;
    }
    case enum_EVENT_MAKE_GRAPHIC: {
      Topbar_makeGraphic(_topbar);
      break;
    }
    default:{}
  }
  return Box_eventProcess(_topbar, event_id, args);
}
