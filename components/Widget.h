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
 * Filename: Widget.h
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

typedef struct IDE IDE;
typedef struct DrawTask DrawTask;

typedef struct Widget Widget;

#define OVERRIDE // means this function not need to call the SUPER's function
#define EXTEND   // means this function needs to call the SUPER's function
#define PASSDOWN // means this function needs to call the same function of some sub-widgets
#define PASSOVER // means this function needs to call the same function of parent

typedef void fn_draw(Widget *widget);
typedef void fn_grap(Widget *widget);
typedef bool fn_area(Widget *widget, uint32_t local_coord[2]);
typedef Widget *fn_subs(Widget *widget, uint32_t local_coord[2]);
typedef void *fn_event(Widget *widget, uint32_t event_id, void *args);

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
  Widget *parent;
  IDE * runtime;
  fn_draw * OVERRIDE funcDraw;
  fn_area * OVERRIDE funcRange;
  fn_subs * OVERRIDE getSubWidget;
  fn_grap * PASSDOWN funcMakeGraph;
  fn_event *  EXTEND funcEventProc;
  DrawTask *drawTask;
  REFER(uint32_t) shader;
  void *   msgData[2];
  /// Normally, using `enum BOX_EDGE` as index for box,
  /// but if property `WP_BOX_AS_GEOMETRY` set on,
  /// using `enum BOX_GEO` as index for box.
  uint32_t box[4];
  uint32_t padding[4];
} Widget;

int32_t IdeWidget_adjust_box(Widget *widget);
bool IdeWidget_testLocal(Widget *widget, uint32_t coord[2]);
int32_t IdeWidget_local2global(Widget *widget, uint32_t coord[2]);
int32_t IdeWidget_global2local(Widget *widget, uint32_t coord[2]);
void IdeWidget_parent2local(Widget *widget, uint32_t coord[2]);
void IdeWidget_local2parent(Widget *widget, uint32_t coord[2]);

void *IdeWidget_eventProcess(Widget *widget, uint32_t event_id, void *args);

#define Widget_getBit(widget_field, bit_field) (widget_field & (bit_field))
#define Widget_setBit(widget_field, bit_field) (widget_field |= (bit_field))
#define Widget_unsetBit(widget_field, bit_field) (widget_field &= ~(bit_field))

#define Widget_getStatus(widget, bit_field) ((widget)->status & (bit_field))
#define Widget_setStatus(widget, bit_field) ((widget)->status |= (bit_field))
#define Widget_unsetStatus(widget, bit_field) ((widget)->status &= ~(bit_field))

#define Widget_getProperty(widget, bit_field) ((widget)->property & (bit_field))
#define Widget_setProperty(widget, bit_field) ((widget)->property |= (bit_field))
#define Widget_unsetProperty(widget, bit_field) ((widget)->property &= ~(bit_field))

#define  Widget_width(widget) ( \
  ((widget)->property & WP_BOX_AS_GEOMETRY) ? (widget)->box[BG_W] : (widget)->box[BE_R] - (widget)->box[BE_L] \
)
#define  Widget_height(widget) ( \
  ((widget)->property & WP_BOX_AS_GEOMETRY) ? (widget)->box[BG_H] : (widget)->box[BE_B] - (widget)->box[BE_T] \
)
#define Widget_left(widget) ((widget)->box[BE_L])
#define Widget_top(widget) ((widget)->box[BE_T])
#define Widget_right(widget) ( \
  ((widget)->property & WP_BOX_AS_GEOMETRY) ? (widget)->box[BG_W] + (widget)->box[BE_L] : (widget)->box[BE_R] \
)
#define Widget_bottom(widget) ( \
  ((widget)->property & WP_BOX_AS_GEOMETRY) ? (widget)->box[BG_H] + (widget)->box[BE_T] : (widget)->box[BE_B] \
)

#define Widget_reGeometry(widget, viewport) do { \
    if ((widget)->funcEventProc) (widget)->funcEventProc(widget, enum_EVENT_RE_GEOMETRY, viewport); \
  } while (false)
#define Widget_makeGraphic(widget) do { \
    if ((widget)->funcMakeGraph) (widget)->funcMakeGraph(widget); \
  } while (false)
#define Widget_draw(widget) do { if ((widget)->funcDraw) (widget)->funcDraw(widget); } while (false)

#endif  // XIDE_WIDGET_H
