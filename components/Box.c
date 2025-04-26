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
 * Filename: Box.c
 * Creator: Yaokai Liu
 * Create Date: 2025-04-15
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "Box.h"
#include "draw.h"


void Box_append(Box *box, Widget *child) {
  if (box->children) {
    Array_append(box->children, &child, 1);
    child->runtimeContext = box->SUPER.runtimeContext;
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
  if (box->corners) { }
  if (!box->children) { return ; }
  uint32_t n_children = Array_length(box->children);
  Widget * const*children = Array_first_real(box->children);
  for (uint32_t i = 0; i < n_children; i++) {
    Widget_draw(children[i]);
  }
}

void ideMakeBox(IDE *ide, Widget *_box) {
  Vertex2D corners[] = {
    {(float) _box->box[BE_L],  (float) _box->box[BE_T], 0x3c3f41ff},
    {(float) _box->box[BE_R], (float) _box->box[BE_T], 0x3c3f41ff},
    {(float) _box->box[BE_R], (float) _box->box[BE_B], 0x3c3f41ff},
    {(float) _box->box[BE_L],  (float) _box->box[BE_B], 0x3c3f41ff},
  };
  Array *vertex_array = Array_new(sizeof(Vertex2D), enum_XGL_COORD, _box->allocator);
  Array_append(vertex_array, corners, 4);
  if (_box->drawTask) { xglDestroyDrawTask(_box->drawTask, ide->allocator); }
  _box->drawTask = ideCreatePolygon2D(vertex_array, 0, true, _box->allocator);
  GLuint *shader = (_box->shader)
                     ? Array_virt2real(ide->shaderProgramArray, _box->shader)
                     :Array_virt2real(ide->shaderProgramArray, ide->defaultShader[DEFAULT_SHADER]);
  xglBindShaderProgram(_box->drawTask, *shader);
  releasePrimeArray(vertex_array);
}

#define testLocal(w, c) ((w) && IdeWidget_testLocal(w, c))
Widget *Box_getSubWidget(Widget *_box, uint32_t local_coord[2]) {
  Box *box = (Box *)_box;
  if (!box->children) { return nullptr; }
  uint32_t n_children = Array_length(box->children);
  Widget * const*children = Array_first_real(box->children);
  for (uint32_t i = 0; i < n_children; i++) {
    if (testLocal(children[i], local_coord)) { return children[i]; }
  }
  return nullptr;
}