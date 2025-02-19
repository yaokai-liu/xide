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
 * Module Name: runtime
 * Filename: utils.c
 * Creator: Yaokai Liu
 * Create Date: 2024-7-7
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "utils.h"
#include "definition.h"
#include <math.h>

void rgba2XGLColor(uint32_t rgba, XGLColor *gl_color) {
  (*gl_color)[CAX_R] = (float) ((rgba >> 0x18) & 255) / 255.0f;
  (*gl_color)[CAX_G] = (float) ((rgba >> 0x10) & 255) / 255.0f;
  (*gl_color)[CAX_B] = (float) ((rgba >> 0x08) & 255) / 255.0f;
  (*gl_color)[CAX_A] = (float) ((rgba >> 0x00) & 255) / 255.0f;
}

float xglNormalize(float *vertex, int dim) {
  float norm = 0;
  for (int i = 0; i < dim; i++) { norm += vertex[i] * vertex[i]; }
  norm = sqrtf(norm);
  for (int i = 0; i < dim; i++) { vertex[i] /= norm; }
  return norm;
}

