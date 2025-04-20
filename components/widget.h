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
 * Filename: widget.h
 * Creator: Yaokai Liu
 * Create Date: 2024-7-6
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_WIDGET_H
#define XIDE_WIDGET_H

#include "array.h"
#include "char_t.h"
#include "enum.h"
#include "shape2d.h"
#include <stdint.h>

typedef struct Widget Widget;

typedef bool fn_area(Widget *widget, uint32_t coord[2]);
typedef uint32_t fn_color(Widget *widget, uint32_t coord[2]);

/**
 ** Suggestion:
 **   1. The geometry object defined by `funcRange`
 **      is suggested more convex and more connected.
 **   2. The `geometry` is suggested smallest that
 **      can only just place `funcRange`.
 **/
typedef struct Widget {
  uint32_t type;
  uint32_t property;
  uint32_t status;
  const Allocator *allocator;
  void * instance; // maybe a virtual address
  uint32_t box[4];
  fn_area *funcRange;
  fn_color *funcColor;
} Widget;

int32_t IdeWidget_adjust_box(Widget *widget);

#endif  // XIDE_WIDGET_H
