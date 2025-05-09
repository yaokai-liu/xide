/* License
 *
 * xide - An integrated development environment
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
 * Module Name: widgets
 * Filename: Box.c
 * Creator: Yaokai Liu
 * Create Date: 2025-04-15
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "Box.h"
#include "draw.h"
#include "runtime-msg.h"

void Box_append(Box *box, Widget *child) {
  if (!box->children) {
     box->children = Array_new(sizeof(Widget *), WIDGET_TYPE_WIDGET, box->SUPER.allocator);
  }
  Array_append(box->children, &child, 1);
}

void Box_draw(Widget *_box) {
  Box *box = (Box *)_box;
  if (_box->drawTask) { ideDraw(_box->runtime, _box->drawTask); }
  if (!box->children) { return ; }
  uint32_t n_children = Array_length(box->children);
  Widget * const*children = Array_first_real(box->children);
  for (uint32_t i = 0; i < n_children; i++) {
    Widget_draw(children[i]);
  }
}

void *Box_eventProcess(Widget *_box, uint32_t event_id, void *args) {
  return IdeWidget_eventProcess(_box, event_id, args);
}

void ideMakeBox(IDE *, Widget *) {}

Widget *Box_curSubWidget(Widget *_box, uint32_t coord[2]) {
  Box *box = (Box *)_box;
  if (!box->children) { return nullptr; }
  uint32_t n_children = Array_length(box->children);
  Widget * const*children = Array_first_real(box->children);
  for (uint32_t i = 0; i < n_children; i++) {
    if (!children[i]) { continue; }
    uint32_t local_coord[2] = {[AXIS_X] = coord[AXIS_X],
                               [AXIS_Y] = coord[AXIS_Y]};
    IdeWidget_parent2local(children[i], local_coord);
    if (IdeWidget_testLocal(children[i], local_coord)) { return children[i]; }
  }
  return nullptr;
}

void Box_makeGraph(Widget *_box) {
  Box *box = (Box *) _box;
  if (box->children) {
    uint32_t n_children = Array_length(box->children);
    Widget * const*children = Array_first_real(box->children);
    for (uint32_t i = 0; i < n_children; i++) {
      Widget_makeGraphic(children[i]);
    }
  }
  ideMakeBox(_box->runtime, _box);
}
