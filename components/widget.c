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

#include "widget.h"
#include "enum.h"
#include "minmax.h"

int32_t IdeWidget_adjust_box(Widget *widget) {
  if (widget->box[BE_LEFT] > widget->box[BE_RIGHT] || widget->box[BE_TOP] > widget->box[BE_BOTTOM]) { return -1; }
  if (widget->funcRange) {
    uint32_t left = widget->box[BE_RIGHT], right = widget->box[BE_LEFT];
    uint32_t top = widget->box[BE_BOTTOM], bottom = widget->box[BE_TOP];
    for (uint32_t i = widget->box[BE_TOP]; i <= widget->box[BE_BOTTOM]; i++) {
      uint32_t j = widget->box[BE_LEFT];
      uint32_t coord[2] = {j, i};
      while (j <= widget->box[BE_RIGHT] && !widget->funcRange(widget, coord)) { j++, coord[0] = j; }
      left = min(left, j);
      if (top >= widget->box[BE_BOTTOM] && widget->funcRange(widget, coord)) { top = i; }
      if (bottom <= widget->box[BE_TOP] && widget->funcRange(widget, coord)) { bottom = i; }
      for (; j <= widget->box[BE_RIGHT]; j++, coord[0] = j) {
        if (widget->funcRange(widget, coord)) { right = max(right, j); }
      }
    }
    // update widget box
    widget->box[BE_LEFT] = max(left, widget->box[BE_LEFT]);
    widget->box[BE_RIGHT] = min(right, widget->box[BE_RIGHT]);
    widget->box[BE_TOP] = max(top, widget->box[BE_TOP]);
    widget->box[BE_BOTTOM] = min(bottom, widget->box[BE_BOTTOM]);
  }
  return 0;
}
