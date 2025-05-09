/* License
 *
 * ${PROJ_DESCRIPTION}
 * Copyright (C) 2025 Yaokai Liu
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
 * Module Name: application
 * Filename: topbar.c
 * Creator: Yaokai Liu
 * Create Date: 2025-05-09
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "topbar.h"
#include "color.h"
#include "runtime.h"
#include "minmax.h"
#define lenof(_array)  (sizeof(_array) / sizeof(_array[0]))

void Topbar_makeGraph(Widget *_topbar) {
  Box_makeGraph(_topbar);
  if (Widget_getProperty(_topbar, WIDGET_PROPERTY_RE_GEO_FROM_CHILDREN)) {
    uint32_t size[4] = {Widget_left(_topbar), Widget_top(_topbar), Widget_width(_topbar), Widget_height(_topbar)};
    Topbar_resize(_topbar, size);
  } else {
    ideMakeTopbar(_topbar->runtime, _topbar);
  }
}

void Topbar_resize(Widget *_topbar, const uint32_t box[4]) {
  Box *topbar = (Box *)_topbar;
  uint32_t stoke = _topbar->padding[BE_T];
  if (topbar->children) {
    uint32_t n_children = Array_length(topbar->children);
    Widget * const*children = Array_first_real(topbar->children);
    for (uint32_t i = 0; i < n_children; i++) {
      // TODO:
      Widget_reGeometry(children[i], (void *) box);
      stoke = max(stoke, Widget_bottom(children[i]));
    }
  }
  _topbar->box[BG_H] = stoke + _topbar->padding[BE_B];
  uint32_t old_height = Widget_height(_topbar);
  if (Widget_getProperty(_topbar, WIDGET_PROPERTY_BOX_AS_GEOMETRY)) {
    uint32_t t_box[4] = {[BE_T] = 0, [BE_B] = min(Widget_width(_topbar->parent), old_height)};
    Window_measureBox((Window *) _topbar->parent, t_box, BE_T);
    box2geo(t_box, _topbar->box);
  } else {
    Window_measureCentral((Window *) _topbar->parent, _topbar->box);
    _topbar->box[BE_T] = 0;
    _topbar->box[BE_B] = min(Widget_width(_topbar->parent), old_height);
  }
  ideMakeTopbar(_topbar->runtime, _topbar);
}

void *Topbar_eventProcess(Widget *_topbar, uint32_t event_id, void *args) {
  switch (event_id) {
    case enum_EVENT_CURSOR_MOVE: {
      uint32_t *global_position = args;
      if (Widget_getStatus(_topbar, WIDGET_STATUS_PRESSED)) {
        uint32_t vector[2] = {
          [AXIS_X] = global_position[AXIS_X] - (uint32_t) (uint64_t) _topbar->msgData[AXIS_X],
          [AXIS_Y] = global_position[AXIS_Y] - (uint32_t) (uint64_t) _topbar->msgData[AXIS_Y],
        };
        ideShiftWindow(_topbar->runtime, vector);
      } else {
        _topbar->msgData[AXIS_X] = (void *) (uint64_t) global_position[AXIS_X];
        _topbar->msgData[AXIS_Y] = (void *) (uint64_t) global_position[AXIS_Y];
      }
      break;
    }
    case enum_EVENT_RE_GEOMETRY: {
      Topbar_resize(_topbar, args);
      break;
    }
    default:{}
  }
  return Box_eventProcess(_topbar, event_id, args);
}

void ideMakeTopbar(IDE *ide, Widget *_topbar) {
  if (_topbar->drawTask) { xglDestroyDrawTask(_topbar->drawTask, ide->allocator); }
  if (!Widget_width(_topbar) || !Widget_height(_topbar)) {
    _topbar->drawTask = nullptr;
    return;
  }
  PixelVertex2D corners[] = {
    {{_topbar->box[BG_X], _topbar->box[BG_Y]}, 0xffffffff},
    {{_topbar->box[BG_X] + _topbar->box[BG_W], _topbar->box[BG_Y]}, 0xffffffff},
    {{_topbar->box[BG_X] + _topbar->box[BG_W], (_topbar->box[BG_Y] + _topbar->box[BG_H])}, 0xffffffff},
    {{_topbar->box[BG_X], (_topbar->box[BG_Y] + _topbar->box[BG_H])}, 0xffffffff},
  };
  Array *vertex_array = Array_new(sizeof(PixelVertex2D), enum_XGL_COORD, _topbar->allocator);
  Array_append(vertex_array, corners, 4);
  _topbar->drawTask = ideCreatePixelPolygon2D(vertex_array, 0, true, _topbar->allocator);
  GLuint *shader = (_topbar->shader)
                     ? Array_virt2real(ide->shaderProgramArray, _topbar->shader)
                     :Array_virt2real(ide->shaderProgramArray, ide->defaultShader[DEFAULT_SHADER]);
  xglBindShaderProgram(_topbar->drawTask, *shader);
  releasePrimeArray(vertex_array);
}

inline void Window_setTextTitle(Window *window, const char_t *title) {
  Box *topbar = window->SUPER.allocator->calloc(1, sizeof(Box));
  topbar->SUPER.type = WIDGET_TYPE_BAR;
  topbar->SUPER.status = WIDGET_STATUS_FOCUSED;
  topbar->SUPER.property = WIDGET_PROPERTY_RE_GEO_TO_CHILDREN
                           | WIDGET_PROPERTY_BOX_AS_GEOMETRY
                           | WIDGET_PROPERTY_CURSOR_CAPTURABLE
                           | WIDGET_PROPERTY_RE_GEO_FROM_CHILDREN;
  topbar->SUPER.allocator = window->SUPER.allocator;
  topbar->SUPER.runtime = window->SUPER.runtime;
  topbar->SUPER.parent = (Widget *) window;
  topbar->SUPER.box[BG_X] = 0;
  topbar->SUPER.box[BG_Y] = 0;
  topbar->SUPER.box[BG_W] = Widget_width((Widget *)window);
  topbar->SUPER.box[BG_H] = 0;
  topbar->SUPER.funcDraw = Box_draw;
  topbar->SUPER.funcRange = nullptr;
  topbar->SUPER.funcMakeGraph = Topbar_makeGraph;
  topbar->SUPER.curSubWidget = Box_curSubWidget;
  topbar->SUPER.funcEventProc = Topbar_eventProcess;
  topbar->SUPER.padding[BE_L] = 10;
  topbar->SUPER.padding[BE_R] = 10;
  topbar->SUPER.padding[BE_T] = 5 ;
  topbar->SUPER.padding[BE_B] = 5 ;

  // set window title
  const Font IDE_DEFAULT_FONT = {.path = "fonts/msyh.ttc", .index = 0, .size = 12};

  Text *text = window->SUPER.allocator->calloc(1, sizeof(Text));
  text->SUPER.type = WIDGET_TYPE_TEXT;
  text->SUPER.status = WIDGET_STATUS_FOCUSED;
  text->SUPER.property = WIDGET_PROPERTY_BOX_ALWAYS_RE_ADJUST;
  text->SUPER.allocator = window->SUPER.allocator;
  text->SUPER.parent = (Widget *) topbar;
  text->SUPER.runtime = window->SUPER.runtime;
  text->SUPER.funcDraw = Text_draw;
  text->SUPER.funcRange = nullptr;
  text->SUPER.curSubWidget = nullptr;
  text->SUPER.funcEventProc = nullptr;
  text->SUPER.funcMakeGraph = Text_makeGraph;
  text->SUPER.box[BE_L] = topbar->SUPER.padding[BE_L];
  text->SUPER.box[BE_T] = topbar->SUPER.padding[BE_T] + 10;
  text->text = title;
  text->font = IDE_DEFAULT_FONT;
  text->mode = TS_RIGHT | TS_V_CENTER | TS_HORIZONTAL;
  text->color = RGBA_BLACK;

  Box_append(topbar, (Widget *) text);

  window->bars[BE_T] = (Widget *) topbar;
}
