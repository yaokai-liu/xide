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

#include <minmax.h>
#include "print.h"
#include "draw.h"
#include "ide.h"
#include "runtime.h"
#include "utils.h"
#include "font-manage.h"

inline void
xglGenCharCoord2D(const CharModel *model, const Vertex2D *center, const TextureAtlas *atlas, XGLVertex dest[4]) {
  constexpr float scale = 0.5f;
  dest[RC_LT].coord[AXIS_X] = ((float) center->coord[AXIS_X]) - ((float) model->size[AXIS_X]) * scale;
  dest[RC_LT].coord[AXIS_Y] = ((float) center->coord[AXIS_Y]) - ((float) model->size[AXIS_Y]) * scale;
  dest[RC_RT].coord[AXIS_X] = ((float) center->coord[AXIS_X]) + ((float) model->size[AXIS_X]) * scale;
  dest[RC_RT].coord[AXIS_Y] = ((float) center->coord[AXIS_Y]) - ((float) model->size[AXIS_Y]) * scale;
  dest[RC_LB].coord[AXIS_X] = ((float) center->coord[AXIS_X]) - ((float) model->size[AXIS_X]) * scale;
  dest[RC_LB].coord[AXIS_Y] = ((float) center->coord[AXIS_Y]) + ((float) model->size[AXIS_Y]) * scale;
  dest[RC_RB].coord[AXIS_X] = ((float) center->coord[AXIS_X]) + ((float) model->size[AXIS_X]) * scale;
  dest[RC_RB].coord[AXIS_Y] = ((float) center->coord[AXIS_Y]) + ((float) model->size[AXIS_Y]) * scale;
  rgba2XGLColor(center->color, &dest[RC_LT].color);
  rgba2XGLColor(center->color, &dest[RC_RT].color);
  rgba2XGLColor(center->color, &dest[RC_LB].color);
  rgba2XGLColor(center->color, &dest[RC_RB].color);
  dest[RC_LT].tex_coord[AXIS_X] = ((float) model->offset - 1);
  dest[RC_LT].tex_coord[AXIS_Y] = 0.0f;
  dest[RC_RT].tex_coord[AXIS_X] = (float) (model->offset + model->size[AXIS_X]);
  dest[RC_RT].tex_coord[AXIS_Y] = 0.0f;
  dest[RC_LB].tex_coord[AXIS_X] = (float) (model->offset - 1);
  dest[RC_LB].tex_coord[AXIS_Y] = (float) model->size[AXIS_Y];
  dest[RC_RB].tex_coord[AXIS_X] = (float) (model->offset + model->size[AXIS_X]);
  dest[RC_RB].tex_coord[AXIS_Y] = (float) model->size[AXIS_Y];
}
