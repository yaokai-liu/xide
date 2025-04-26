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
 * Filename: Widget.c
 * Creator: Yaokai Liu
 * Create Date: 2024-7-6
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "Widget.h"
#include "enum.h"
#include "minmax.h"
#include "runtime-enum.h"
uint32_t *geo2box(const uint32_t * restrict geo, uint32_t * restrict box);
uint32_t *box2geo(const uint32_t * restrict box, uint32_t * restrict geo);

inline uint32_t *geo2box(const uint32_t * restrict geo, uint32_t * restrict box) {
  box[BE_L] = geo[BG_X];
  box[BE_T] = geo[BG_Y];
  box[BE_R] = geo[BG_X] + geo[BG_W];
  box[BE_B] = geo[BG_Y] + geo[BG_H];
  return box;
}

inline uint32_t *box2geo(const uint32_t * restrict box, uint32_t * restrict geo) {
  geo[BG_X] = box[BE_L];
  geo[BG_Y] = box[BE_T];
  geo[BG_W] = box[BE_R] - box[BE_L];
  geo[BG_H] = box[BE_B] - box[BE_T];
  return geo;
}

int32_t IdeWidget_adjust_box(Widget *widget) {
  if (widget->type & WT_CUSTOM_WIDGET) { return -1; }
  uint32_t *box = widget->box;
  if (box[BE_L] > box[BE_R] || box[BE_T] > box[BE_B]) { return -1; }

  if (!widget->funcRange) { return 0; }

  uint32_t BOX[4] = {};
  box = (widget->property & WP_BOX_AS_GEOMETRY) ? geo2box(widget->box, BOX) : widget->box;
  uint32_t left = box[BE_R], right = box[BE_L];
  uint32_t top = box[BE_B], bottom = box[BE_T];
  for (uint32_t i = box[BE_T]; i <= box[BE_B]; i++) {
    uint32_t j = box[BE_L];
    uint32_t coord[2] = {j, i};
    while (j <= box[BE_R] && !widget->funcRange(widget, coord)) { j++, coord[0] = j; }
    left = min(left, j);
    if (top >= box[BE_B] && widget->funcRange(widget, coord)) { top = i; }
    if (bottom <= box[BE_T] && widget->funcRange(widget, coord)) { bottom = i; }
    for (; j <= box[BE_R]; j++, coord[0] = j) {
      if (widget->funcRange(widget, coord)) { right = max(right, j); }
    }
  }
  // update widget box
  box[BE_L] = max(left, box[BE_L]);
  box[BE_R] = min(right, box[BE_R]);
  box[BE_T] = max(top, box[BE_T]);
  box[BE_B] = min(bottom, box[BE_B]);
  (widget->property & WP_BOX_AS_GEOMETRY) ? box2geo(box, widget->box) : widget->box;

  return 0;
}

inline int32_t IdeWidget_local2global(Widget *widget, uint32_t coord[2]) {
  do {
    coord[AXIS_X] += widget->box[BE_L];
    coord[AXIS_Y] += widget->box[BE_T];
    if (widget->property & WP_PARENT_REFER) { return 1; }
    widget = widget->parent;
  } while (widget);
  return 0;
}

inline int32_t IdeWidget_global2local(Widget *widget, uint32_t coord[2]) {
  do {
    coord[AXIS_X] -= widget->box[BE_L];
    coord[AXIS_Y] -= widget->box[BE_T];
    if (widget->property & WP_PARENT_REFER) { return 1; }
    widget = widget->parent;
  } while (widget);
  return 0;
}

inline int32_t IdeWidget_parent2local(Widget *widget, uint32_t coord[2]) {
  coord[AXIS_X] -= widget->box[BE_L];
  coord[AXIS_Y] -= widget->box[BE_T];
  return 0;
}
inline int32_t IdeWidget_local2parent(Widget *widget, uint32_t coord[2]) {
  coord[AXIS_X] += widget->box[BE_L];
  coord[AXIS_Y] += widget->box[BE_T];
  return 0;
}

inline bool IdeWidget_testLocal(Widget *widget, uint32_t coord[2]) {
  bool in_box = coord[AXIS_X] <= Widget_width(widget)
             && coord[AXIS_Y] <= Widget_height(widget);
  if (!widget->funcRange) { return in_box; }
  return in_box && widget->funcRange(widget, coord);
}

void *IdeWidget_eventProcess(Widget *widget, uint32_t event_id, void *args) {
  switch (event_id) {
    case enum_EVENT_CURSOR_ENTER: {
      widget->status |= WS_HOVERED;
      return nullptr;
    }
    case enum_EVENT_CURSOR_LEAVE: {
      widget->status &= ~WS_HOVERED;
      return nullptr;
    }
    default:{ return nullptr; }
  }
}
