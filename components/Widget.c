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
 * Filename: widget.c
 * Creator: Yaokai Liu
 * Create Date: 2024-7-6
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "Widget.h"
#include "enum.h"
#include "minmax.h"
uint32_t *geo2box(const uint32_t * restrict geo, uint32_t * restrict box);
uint32_t *box2geo(const uint32_t * restrict box, uint32_t * restrict geo);

inline uint32_t *geo2box(const uint32_t * restrict geo, uint32_t * restrict box) {
  box[BE_LEFT] = geo[BG_X];
  box[BE_TOP] = geo[BG_Y];
  box[BE_RIGHT] = geo[BG_X] + geo[BG_W];
  box[BE_BOTTOM] = geo[BG_Y] + geo[BG_H];
  return box;
}

inline uint32_t *box2geo(const uint32_t * restrict box, uint32_t * restrict geo) {
  geo[BG_X] = box[BE_LEFT];
  geo[BG_Y] = box[BE_TOP];
  geo[BG_W] = box[BE_RIGHT] - box[BE_LEFT];
  geo[BG_H] = box[BE_BOTTOM] - box[BE_TOP];
  return geo;
}

int32_t IdeWidget_adjust_box(Widget *widget) {
  if (widget->type & WT_CUSTOM_WIDGET) { return -1; }
  uint32_t *box = widget->box;
  if (box[BE_LEFT] > box[BE_RIGHT] || box[BE_TOP] > box[BE_BOTTOM]) { return -1; }

  if (!widget->funcRange) { return 0; }

  uint32_t BOX[4] = {};
  box = (widget->property & WP_BOX_AS_GEOMETRY) ? geo2box(widget->box, BOX) : widget->box;
  uint32_t left = box[BE_RIGHT], right = box[BE_LEFT];
  uint32_t top = box[BE_BOTTOM], bottom = box[BE_TOP];
  for (uint32_t i = box[BE_TOP]; i <= box[BE_BOTTOM]; i++) {
    uint32_t j = box[BE_LEFT];
    uint32_t coord[2] = {j, i};
    while (j <= box[BE_RIGHT] && !widget->funcRange(widget, coord)) { j++, coord[0] = j; }
    left = min(left, j);
    if (top >= box[BE_BOTTOM] && widget->funcRange(widget, coord)) { top = i; }
    if (bottom <= box[BE_TOP] && widget->funcRange(widget, coord)) { bottom = i; }
    for (; j <= box[BE_RIGHT]; j++, coord[0] = j) {
      if (widget->funcRange(widget, coord)) { right = max(right, j); }
    }
  }
  // update widget box
  box[BE_LEFT] = max(left, box[BE_LEFT]);
  box[BE_RIGHT] = min(right, box[BE_RIGHT]);
  box[BE_TOP] = max(top, box[BE_TOP]);
  box[BE_BOTTOM] = min(bottom, box[BE_BOTTOM]);
  (widget->property & WP_BOX_AS_GEOMETRY) ? box2geo(box, widget->box) : widget->box;

  return 0;
}

void IdeWidget_append(Widget *widget, Widget *child) {
  if ((widget->property & WP_CHILD_CHILDREN) && widget->child.children) {
    Array_append(widget->child.children, &child, 1);
    child->parent = widget;
  }
}

int32_t IdeWidget_local2global(Widget *widget, uint32_t coord[2]) {
  if (!widget) { return 0; }
  do {
    coord[AXIS_X] += widget->box[BE_LEFT];
    coord[AXIS_Y] += widget->box[BE_TOP];
    if (widget->property & WP_PARENT_REFER) {
      return -1;
    } else {
      widget = widget->parent;
    }
  } while (widget);
  return 1;
}
