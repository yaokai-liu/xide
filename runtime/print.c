/* License
 *
 * xide - An integrated development environment
 * Copyright (C) 2025 Yaokai Liu
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
 * Filename: print.c
 * Creator: Yaokai Liu
 * Create Date: 2025-02-22
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "print.h"
#include "draw.h"
#include "font-manage.h"
#include "ide.h"
#include "minmax.h"
#include "runtime.h"
#include "utils.h"

inline void xglGenCharCoord2D(const CharModel *model, const Vertex2D *center, const TextureAtlas *atlas,
                              XGLVertex dest[4]) {
  constexpr float scale = 0.5f;
  dest[BC_LT].coord[AXIS_X] = ((float) center->coord[AXIS_X]) - ((float) model->size[AXIS_X]) * scale;
  dest[BC_LT].coord[AXIS_Y] = ((float) center->coord[AXIS_Y]) - ((float) model->size[AXIS_Y]) * scale;
  dest[BC_RT].coord[AXIS_X] = ((float) center->coord[AXIS_X]) + ((float) model->size[AXIS_X]) * scale;
  dest[BC_RT].coord[AXIS_Y] = ((float) center->coord[AXIS_Y]) - ((float) model->size[AXIS_Y]) * scale;
  dest[BC_LB].coord[AXIS_X] = ((float) center->coord[AXIS_X]) - ((float) model->size[AXIS_X]) * scale;
  dest[BC_LB].coord[AXIS_Y] = ((float) center->coord[AXIS_Y]) + ((float) model->size[AXIS_Y]) * scale;
  dest[BC_RB].coord[AXIS_X] = ((float) center->coord[AXIS_X]) + ((float) model->size[AXIS_X]) * scale;
  dest[BC_RB].coord[AXIS_Y] = ((float) center->coord[AXIS_Y]) + ((float) model->size[AXIS_Y]) * scale;
  rgba2XGLColor(center->color, &dest[BC_LT].color);
  rgba2XGLColor(center->color, &dest[BC_RT].color);
  rgba2XGLColor(center->color, &dest[BC_LB].color);
  rgba2XGLColor(center->color, &dest[BC_RB].color);
  dest[BC_LT].tex_coord[AXIS_X] = ((float) model->offset - 1);
  dest[BC_LT].tex_coord[AXIS_Y] = 0.0f;
  dest[BC_RT].tex_coord[AXIS_X] = (float) (model->offset + model->size[AXIS_X]);
  dest[BC_RT].tex_coord[AXIS_Y] = 0.0f;
  dest[BC_LB].tex_coord[AXIS_X] = (float) (model->offset - 1);
  dest[BC_LB].tex_coord[AXIS_Y] = (float) model->size[AXIS_Y];
  dest[BC_RB].tex_coord[AXIS_X] = (float) (model->offset + model->size[AXIS_X]);
  dest[BC_RB].tex_coord[AXIS_Y] = (float) model->size[AXIS_Y];
}
