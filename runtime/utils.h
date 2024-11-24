/**
 * Project Name: xide
 * Module Name: runtime
 * Filename: utils.h
 * Creator: Yaokai Liu
 * Create Date: 2024-7-7
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_UTILS_H
#define XIDE_UTILS_H

#include "glad/glad.h"
#include "xgl-object.h"

float XGL_normalize(float *vertex, int dim);

void rgba2XGLColor(uint32_t rgba, XGLColor *gl_color);

#endif  // XIDE_UTILS_H
