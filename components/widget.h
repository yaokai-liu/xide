/**
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
#include "shape2D.h"
#include <stdint.h>

typedef struct Widget {
  Array *subWidgets;
} Widget;

#endif  // XIDE_WIDGET_H
