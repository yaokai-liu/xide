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
#include "shape2D.h"
#include <stdint.h>

typedef struct ColorGroup {
  uint32_t plain;
  uint32_t onFocus;
  uint32_t onHover;
  uint32_t onClick;
} IDEColorGroup;

#define STRUCT_WIDGET                    \
  uint32_t type;                         \
  int geometry[4];                       \
  float viewport[4];                     \
  uint32_t colors[4];                    \
  Array /* <DrawTask> */ *drawTaskArray; \
  Array /* <Widget> */ *subWidgets

typedef struct Widget {
  STRUCT_WIDGET;
} Widget;

typedef struct ToolBar {
  STRUCT_WIDGET;
  int direction;
} ToolBar;

typedef struct RollBar {
  uint32_t width;
  IDEColorGroup colors[2];
} IDERollBar;

#endif  // XIDE_WIDGET_H
