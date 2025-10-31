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
 * Module Name: widgets
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
#include "graphic.h"
#include "shape2d.h"
#include "inherit.h"
#include "ide-types.h"
#include <stdint.h>


typedef void fn_update(Widget *widget);
typedef void fn_draw(const Widget *widget);
typedef void *fn_event(Widget *widget, uint32_t event_id, void *args);
typedef Widget *fn_subs(const Widget *widget, uint32_t local_coord[2]);

/**
 * @Notice
 ** If the `type` field's `WIDGET_TYPE_CUSTOM_WIDGET` set on,
 **   1. `property` will be interpreted as a pointer to a custom defined address;
 **   2. fields after `parent` will not be access by default widget methods.
 **      These fields can be used for other purpose if necessary.
 **
 ** @Suggestion
 **   1. The geometry object defined by `funcRange`
 **      is suggested most convex and most connected.
 **   2. The `geometry` is suggested smallest that
 **      can only just place `funcRange`.
 **/
typedef struct Widget {
  /**
   * @description
   * Type of the widget.
   */
  uint32_t type;
  /**
   * @description
   * Status of the widget to determine the widget's behaviors.
   */
  uint32_t status;
  /**
   * @description
   * the Identify Number of the widget.
   */
  uint64_t ident;
  /**
   * @description
   * Property of the widget to determine the widget's behaviors.
   * @escape
   * If the `type` field's `WIDGET_TYPE_CUSTOM_WIDGET` set on,
   * this field will be a pointer to a custom defined address.
   */
  uint64_t property;
  /**
   * @description Memory allocator of widget.
   */
  const Allocator *allocator;
  /**
   * @description Parent widget of the widget.
   */
  Widget *parent;
  /**
   * @description IDE runtime of the widget.
   */
  IDE * runtime;
  /**
   * @description
   * Return a sub-widget the position is in.
   * @tags OVERRIDE NONEPASS
   * @param widget the widget itself
   * @param local_coord the given position,
   * has been converted to widget local coord
   */
  fn_subs * OVERRIDE NONEPASS curSubWidget;
  /**
   * @description
   * Process event that happened on the widget.
   * Some of the events may pass to its parent.
   * @tags EXTEND PASSOVER
   * @param widget the widget itself
   * @param event_id the event id
   * @param args arguments may be passed
   */
  fn_event *  EXTEND PASSOVER funcEventProc;
  /**
   * @descriptionp
   * Prepare or update the draw task, must pass to all widgets depend on it.
   * Usually called when the UI have to update.
   * @tags OVERRIDE PASSDOWN
   * @param widget the widget itself
   */
  fn_update * OVERRIDE PASSDOWN funcUpdateGraph;
  /**
   * @description
   * The real draw command, must pass to all sub-widgets.
   * @tags OVERRIDE PASSDOWN
   * @param widget the widget itself
   */
  fn_draw * OVERRIDE PASSDOWN funcDraw;
  /**
   * @description   the graphic interface of the widget
   * User can choose different graphic to change the graphic of the widget.
   * This graphic determines the widget's range to response cursor event,
   * the widget's shape and style, and the real draw command.
   * @escape        if is nullptr,
   * means this widget will make directly by the `funcUpdateGraph`
   */
  Graphic *graphic;
  /**
   * @description the draw task of the graphic
   */
  DrawTask *drawTask;
  /**
   * @description the last updating time
   * which is the number of times external input entered.
   */
  uint64_t timestamp;
  /**
   * @description   widgets that graphics depend on this widget
   * If this widget changed its UI, include box or geometry, graphics, range,
   * all the linkages should update.
   * @type          Array<Widget *>
   */
  Array *linkages;
  /**
   * @description Temporary datas to keep.
   */
  void *   msgData[2];
  /**
   * @Description
   * The geometry box of the widget.
   * @normally
   * left, top, right and bottom sides of the widget.
   * @escape
   * If widget property `WIDGET_PROPERTY_BOX_AS_GEOMETRY` is set on,
   * data in the box will be interpreted as position
   * of left top corner, and size of the box.
   * @behaivor
   * If widget property `WIDGET_PROPERTY_BOX_ALWAYS_RE_ADJUST` is set on,
   * the box size will always change after make graphic.
   */
  uint32_t box[4];
  /**
   * @description
   * left, top, right and bottom paddings of the widget.
   */
  uint32_t padding[4];
} Widget;

int32_t IdeWidget_adjust_box(Widget *widget);
bool IdeWidget_testLocal(const Widget *widget, uint32_t coord[2]);
int32_t IdeWidget_local2global(const Widget *widget, uint32_t coord[2]);
int32_t IdeWidget_global2local(const Widget *widget, uint32_t coord[2]);
void IdeWidget_parent2local(const Widget *widget, uint32_t coord[2]);
void IdeWidget_local2parent(const Widget *widget, uint32_t coord[2]);

void *IdeWidget_eventProcess(Widget *widget, uint32_t event_id, void *args);

#define Widget_getBit(widget_field, bit_field) ((widget_field) & (bit_field))
#define Widget_setBit(widget_field, bit_field) ((widget_field) |= (bit_field))
#define Widget_unsetBit(widget_field, bit_field) ((widget_field) &= ~(bit_field))

#define Widget_getStatus(widget, bit_field) ((widget)->status & (bit_field))
#define Widget_setStatus(widget, bit_field) ((widget)->status |= (bit_field))
#define Widget_unsetStatus(widget, bit_field) ((widget)->status &= ~(bit_field))

#define Widget_getProperty(widget, bit_field) ((widget)->property & (bit_field))
#define Widget_setProperty(widget, bit_field) ((widget)->property |= (bit_field))
#define Widget_unsetProperty(widget, bit_field) ((widget)->property &= ~(bit_field))

#define Widget_setWidth(widget, width) ( \
  ((widget)->property & WIDGET_PROPERTY_BOX_AS_GEOMETRY) ? ((widget)->box[BG_W] = (width)) : ((widget)->box[BE_R] = (widget)->box[BE_L] + (width)) \
)
#define Widget_setHeight(widget, width) ( \
  ((widget)->property & WIDGET_PROPERTY_BOX_AS_GEOMETRY) ? ((widget)->box[BG_H] = (width)) : ((widget)->box[BE_B] = (widget)->box[BE_T] + (width)) \
)

#define  Widget_width(widget) ( \
  ((widget)->property & WIDGET_PROPERTY_BOX_AS_GEOMETRY) ? (widget)->box[BG_W] : (widget)->box[BE_R] - (widget)->box[BE_L] \
)
#define  Widget_height(widget) ( \
  ((widget)->property & WIDGET_PROPERTY_BOX_AS_GEOMETRY) ? (widget)->box[BG_H] : (widget)->box[BE_B] - (widget)->box[BE_T] \
)
#define Widget_left(widget) ((widget)->box[BE_L])
#define Widget_top(widget) ((widget)->box[BE_T])
#define Widget_right(widget) ( \
  ((widget)->property & WIDGET_PROPERTY_BOX_AS_GEOMETRY) ? (widget)->box[BG_W] + (widget)->box[BE_L] : (widget)->box[BE_R] \
)
#define Widget_bottom(widget) ( \
  ((widget)->property & WIDGET_PROPERTY_BOX_AS_GEOMETRY) ? (widget)->box[BG_H] + (widget)->box[BE_T] : (widget)->box[BE_B] \
)

#define Widget_reGeometry(widget, viewport) do { \
    if ((widget)->funcEventProc) (widget)->funcEventProc(widget, enum_EVENT_RE_GEOMETRY, viewport); \
  } while (false)
#define Widget_updateGraphic(widget) do { \
    if ((widget)->funcUpdateGraph) (widget)->funcUpdateGraph(widget); \
  } while (false)
#define Widget_draw(widget) do { if ((widget)->funcDraw) (widget)->funcDraw(widget); } while (false)

void Widget_geo2box(Widget *widget);
void Widget_box2geo(Widget *widget);

uint32_t *geo2box(const uint32_t * restrict geo, uint32_t * restrict box);
uint32_t *box2geo(const uint32_t * restrict box, uint32_t * restrict geo);

#endif  // XIDE_WIDGET_H
