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
 * Module Name: components
 * Filename: container.c
 * Creator: Yaokai Liu
 * Create Date: 2025-04-15
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "Box.h"
#include "draw.h"


void Box_append(Box *box, Widget *child) {
  if (box->children) {
    Array_append(box->children, &child, 1);
    child->parent = (Widget *) box;
  }
}
void Box_update(Widget *_box) {
  Box *box = (Box *)_box;
  if (!(_box->property & WP_RE_GEO_TO_CHILDREN)) { return ; }
  if (!box->children) { return ; }
  uint32_t n_children = Array_length(box->children);
  Widget * const*children = Array_first_real(box->children);
  for (uint32_t i = 0; i < n_children; i++) {
    Widget_update(children[i]);
  }
}

void Box_draw(Widget *_box) {
  Box *box = (Box *)_box;
  if (_box->drawTask) { ideDraw(_box->drawTask, _box->runtimeContext); }
  if (box->borderVertices) { }
  if (!box->children) { return ; }
  uint32_t n_children = Array_length(box->children);
  Widget * const*children = Array_first_real(box->children);
  for (uint32_t i = 0; i < n_children; i++) {
    Widget_draw(children[i]);
  }
}
