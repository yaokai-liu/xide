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
 * Filename: main-content.c
 * Creator: Yaokai Liu
 * Create Date: 2025-05-09
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "main-content.h"

inline void Window_setMainContent(Window *window, Widget *) {
  Box *central = window->SUPER.allocator->calloc(1, sizeof(Box));
  central->SUPER.type = WIDGET_TYPE_BOX;
  central->SUPER.status = WIDGET_STATUS_FOCUSED;
  central->SUPER.property = WIDGET_PROPERTY_RE_GEO_TO_CHILDREN;
  central->SUPER.allocator = window->SUPER.allocator;
  central->SUPER.runtime = window->SUPER.runtime;
  central->SUPER.parent = (Widget *) window;
  central->SUPER.funcDraw = Box_draw;
  central->SUPER.funcRange = nullptr; // default range
  central->SUPER.funcMakeGraph = Central_makeGraph;
  central->SUPER.curSubWidget = Box_curSubWidget;
  central->SUPER.funcEventProc = nullptr; // not accept events
  central->SUPER.padding[BE_L] = 0;
  central->SUPER.padding[BE_R] = 0;
  central->SUPER.padding[BE_T] = 0;
  central->SUPER.padding[BE_B] = 0;
  Window_measureCentral(window, central->SUPER.box);
  Widget *_central = (Widget *)central;
  Widget_box2geo(_central);

  window->central = _central;
}

void Central_makeGraph(Widget *_central) {
  Box_makeGraph(_central);
  ideMakeCentral(_central->runtime, _central);
}

void Central_resize(Widget *_central, const uint32_t [4]) {
  if (Widget_getProperty(_central, WIDGET_PROPERTY_BOX_AS_GEOMETRY)) {
    uint32_t t_box[4] = {};
    Window_measureCentral((Window *) _central->parent, t_box);
    box2geo(t_box, _central->box);
  } else {
    Window_measureCentral((Window *) _central->parent, _central->box);
  }
  Box *central = (Box *)_central;
  if (central->children) {
    uint32_t n_children = Array_length(central->children);
    Widget * const*children = Array_first_real(central->children);
    for (uint32_t i = 0; i < n_children; i++) {
      Widget_reGeometry(children[i], _central->box);
    }
  }
  ideMakeBox(_central->runtime, _central);
}

void ideMakeCentral(IDE *, Widget *) {}
