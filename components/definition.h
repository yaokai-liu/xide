/**
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
  DIRECTION_TOP = 0,
  DIRECTION_LEFT = 1,
  DIRECTION_RIGHT = 2,
  DIRECTION_BOTTOM = 3
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

enum CONFIG {
  CONFIG_X = 0,
  CONFIG_Y = 1,
  CONFIG_W = 2,
  CONFIG_H = 3
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
