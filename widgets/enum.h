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
 * Filename: enum.h
 * Creator: Yaokai Liu
 * Create Date: 2024-7-7
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_ENUM_H
#define XIDE_ENUM_H

// Do not promise that direction order will be clockwise or counter-clockwise.
enum DISTRIBUTE_DIRECTION {
  DIRECTION_C = 0,
  DIRECTION_TC = 1,
  DIRECTION_BC = 2,
  DIRECTION_LC = 3,
  DIRECTION_RC = 4,
  DIRECTION_TL = 5,
  DIRECTION_TR = 6,
  DIRECTION_BL = 7,
  DIRECTION_BR = 8
};

enum AXIS {
  AXIS_X = 0,
  AXIS_Y = 1,
  AXIS_Z = 2,
  AXIS_W = 3
};

enum CAX {
  CAX_R = 0,
  CAX_G = 1,
  CAX_B = 2,
  CAX_A = 3
};

enum BOX_EDGE {
  BE_L,
  BE_T,
  BE_R,
  BE_B
};

enum BOX_CORNER {
  BC_LT,
  BC_LB,
  BC_RT,
  BC_RB
};

enum BOX_GEO {
  BG_X = BE_L,
  BG_Y = BE_T,
  BG_W = BE_R,
  BG_H = BE_B,
};

enum CONFIG {
  CONFIG_X = 0,
  CONFIG_Y = 1,
  CONFIG_W = 2,
  CONFIG_H = 3
};

// Do not promise that direction order will be clockwise or counter-clockwise.
enum VERTEX_ORDER {
  // rectangle vertex order
  VERT_LT = 0,
  VERT_RT = 1,
  VERT_RB = 2,
  VERT_LB = 3,
  // straight line vertex order
  VERT_BEGIN = 0,
  VERT_END = 1,
};

typedef enum IDEWidgetTypeEnum : uint32_t {
  WIDGET_TYPE_NONE = 0,
  WIDGET_TYPE_WIDGET,
  WIDGET_TYPE_WINDOW,
  WIDGET_TYPE_VIEW,
  WIDGET_TYPE_ITEM,
  WIDGET_TYPE_TEXT,
  WIDGET_TYPE_BOX,
  WIDGET_TYPE_BAR,

  // Re-interpret the widget as custom defined.
  WIDGET_TYPE_CUSTOM_WIDGET = 0x80000000
} IDEWidgetTypeEnum;

typedef enum IDEWidgetPropertyEnum : uint64_t {
  WIDGET_PROPERTY_NONE = 0x00LL,

  // `parent` field is a virtual pointer
  WIDGET_PROPERTY_PARENT_REFER = 0x01,

  WIDGET_PROPERTY_CURSOR_CAPTURABLE = 0x02,

  // Box field interpret as geometry.
  WIDGET_PROPERTY_BOX_AS_GEOMETRY = 0x01LL << 8,
  // Box always adjust to widget range.
  WIDGET_PROPERTY_BOX_ALWAYS_RE_ADJUST = 0x02LL << 8,
  // Geometry changed will influence the parent.
  WIDGET_PROPERTY_RE_GEO_TO_PARENT = 0x01LL << 16,
  // Geometry changed will influence children.
  WIDGET_PROPERTY_RE_GEO_TO_CHILDREN = 0x02LL << 16,
  // Geometry changed will influence children.
  WIDGET_PROPERTY_RE_GEO_FROM_CHILDREN = 0x04LL << 16,
} IDEWidgetPropertyEnum;

typedef enum IDEWidgetStatusEnum : uint32_t {
  WIDGET_STATUS_NORMAL  = 0x00,
  WIDGET_STATUS_FOCUSED = 0x01,
  WIDGET_STATUS_HOVERED = 0x02,
  WIDGET_STATUS_PRESSED = 0x04,
  WIDGET_STATUS_CAPTURE = 0x08,
  WIDGET_STATUS_HIDDEN  = 0x10,
} IDEWidgetStatusEnum;

typedef enum IDEBoxTypeEnum : uint32_t {
  BOX_TYPE_PLAIN_BOX = 0,
  BOX_TYPE_HORIZONTAL_LIST,
  BOX_TYPE_VERTICAL_LIST
} IDEBoxTypeEnum;

typedef enum IDEWindowPropertyEnum : uint32_t {
  WINDOW_PROPERTY_LEFT_FULL = 0x01,
  WINDOW_PROPERTY_RIGHT_FULL = 0x02
} IDEWindowPropertyEnum;


#endif  // XIDE_ENUM_H
