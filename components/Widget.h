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

typedef struct DrawTask DrawTask;

typedef struct Widget Widget;

typedef void fn_update(Widget *widget);
typedef bool fn_area(Widget *widget, uint32_t coord[2]);
typedef uint32_t fn_color(Widget *widget, uint32_t coord[2]);

/**
 ** If the `type` field's `WT_CUSTOM_WIDGET` set on,
 **   1. `property` will be interpret as a pointer to a custom defined address;
 **   2. fields after `parent` will not be access by default widget methods.
 **      These fields can be used for other purpose if necessary.
 **
 ** Suggestion:
 **   1. The geometry object defined by `funcRange`
 **      is suggested most convex and most connected.
 **   2. The `geometry` is suggested smallest that
 **      can only just place `funcRange`.
 **/
typedef struct Widget {

  uint32_t type;

  uint32_t status;

  /**
   ** If the `type` field's `WT_CUSTOM_WIDGET` set on,
   ** this field will be a pointer to a custom defined address.
   **/
  uint64_t property;

  const Allocator *allocator;

  const uint8_t *msgBuffer;

  DrawTask *drawTask;

  Widget *parent;

  union {
    Widget *child;
    Array  *children; // Array<Widget *>
  } child;

  /// Normally, using `enum BOX_EDGE` as index for box,
  /// but if property `WP_BOX_AS_GEOMETRY` set on,
  /// using `enum BOX_GEO` as index for box.
  uint32_t box[4];


  // basic methods of widget.

  fn_area *funcRange;
  fn_color *funcColor;
  fn_update *funcUpdate;
} Widget;

void IdeWidget_append(Widget *widget, Widget *child);

int32_t IdeWidget_adjust_box(Widget *widget);
int32_t IdeWidget_local2global(Widget *widget, uint32_t coord[2]);

#define  Widget_width(widget) ( \
  ((widget)->property & WP_BOX_AS_GEOMETRY) ? (widget)->box[BG_W] : (widget)->box[BE_RIGHT] - (widget)->box[BE_LEFT] \
)
#define  Widget_height(widget) ( \
  ((widget)->property & WP_BOX_AS_GEOMETRY) ? (widget)->box[BG_H] : (widget)->box[BE_BOTTOM] - (widget)->box[BE_TOP] \
)
#endif  // XIDE_WIDGET_H
