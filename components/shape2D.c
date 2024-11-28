/**
 * Project Name: xide
 * Module Name: components
 * Filename: shape.c
 * Creator: Yaokai Liu
 * Create Date: 2024-7-9
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "shape2D.h"
#include "definition.h"
#include <math.h>

inline float SLine_length(const Line line) {
  uint32_t len_sq = (line[1].coord[AXIS_X] - line[0].coord[AXIS_X])
                      * (line[1].coord[AXIS_X] - line[0].coord[AXIS_X])
                    + (line[1].coord[AXIS_Y] - line[0].coord[AXIS_Y])
                        * (line[1].coord[AXIS_Y] - line[0].coord[AXIS_Y]);
  return sqrtf((float) len_sq);
}
