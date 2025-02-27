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
 * Filename: definition.h
 * Creator: Yaokai Liu
 * Create Date: 2024-7-7
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_DEFINITION_H
#define XIDE_DEFINITION_H

// Do not promise that direction order will be clockwise or counter-clockwise.
enum DIRECTION {
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

enum RECT_CORNER { RC_LT, RC_LB, RC_RT, RC_RB };

enum CONFIG {
  CONFIG_X = 0,
  CONFIG_Y = 1,
  CONFIG_W = 2,
  CONFIG_H = 3
};

enum WIDGET_STATUS {
  PLAIN = 0,
  ON_FOCUS = 1,
  ON_HOVER = 2,
  ON_CLICK = 3,
};

// Do not promise that direction order will be clockwise or counter-clockwise.
enum VERTEX {
  VERTEX_LT = 0,
  VERTEX_RT = 1,
  VERTEX_RB = 2,
  VERTEX_LB = 3,
  VERTEX_BEGIN = 0,
  VERTEX_END = 1,
};

#endif  // XIDE_DEFINITION_H
