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

float SLine_length(Line *line) {
  uint32_t len_sq =
    (line->end[AXIS_X] - line->start[AXIS_X]) * (line->end[AXIS_X] - line->start[AXIS_X])
    + (line->end[AXIS_Y] - line->start[AXIS_Y]) * (line->end[AXIS_Y] - line->start[AXIS_Y]);
  return sqrtf((float) len_sq);
}
